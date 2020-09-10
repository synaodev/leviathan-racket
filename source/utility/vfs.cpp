#include "./vfs.hpp"
#include "./logger.hpp"
#include "./setup_file.hpp"

#if defined(LEVIATHAN_PLATFORM_WINDOWS)
	#include <windows.h>
#elif defined(LEVIATHAN_PLATFORM_MACOS)
	#include <libproc.h>
#endif

#ifdef LEVIATHAN_POSIX_COMPLIANT
	#include <dirent.h>
	#include <unistd.h>
	#include <sys/stat.h>
	#include <sys/types.h>
#endif

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <streambuf>

#ifndef LEVIATHAN_TOOLCHAIN_APPLECLANG
	#include <filesystem>
#endif

#include <nlohmann/json.hpp>
#include <SDL2/SDL_filesystem.h>

#define SYNAO_SIZEOF_ARRAY(ARR) (sizeof( ARR ) / sizeof( ARR [0] ))

static constexpr byte_t kEventPath[]	= "./data/event/";
static constexpr byte_t kFieldPath[]	= "./data/field/";
static constexpr byte_t kFontPath[]		= "./data/font/";
static constexpr byte_t kI18NPath[]		= "./data/i18n/";
static constexpr byte_t kImagePath[]	= "./data/image/";
static constexpr byte_t kNoisePath[]	= "./data/noise/";
static constexpr byte_t kPalettePath[]	= "./data/palette/";
static constexpr byte_t kSpritePath[]	= "./data/sprite/";
static constexpr byte_t kTileKeyPath[]	= "./data/tilekey/";
static constexpr byte_t kTunePath[]		= "./data/tune/";
static constexpr byte_t kInitExt[]		= "init/";
static constexpr byte_t kSaveExt[]		= "save/";
static constexpr byte_t kOrgName[] 		= "studio-synao";
static constexpr byte_t kAppName[] 		= "leviathan";
static constexpr byte_t kDefaultLang[] 	= "english";
static constexpr arch_t kTotalThreads 	= 4;
static constexpr arch_t kDebugFontIndex = 4;

#if defined(LEVIATHAN_EXECUTABLE_NAOMI)
	vfs_t::vfs_t() :
		thread_pool(),
		storage_mutex(),
		personal(),
		language(kDefaultLang),
		i18n(),
		textures(),
		palettes(),
		shaders(),
		noises(),
		fonts(),
		animations() {}
#else
	vfs_t::vfs_t() :
		thread_pool(),
		storage_mutex(),
		personal(),
		language(kDefaultLang),
		i18n(),
		textures(),
		palettes(),
		shaders() {}
#endif

vfs_t::~vfs_t() {
	if (vfs::device != nullptr) {
		if (vfs::device == this) {
			vfs::device = nullptr;
		} else {
			synao_log("Error! There should not be more than one virtual filesystem!\n");
		}
	}
}

bool vfs_t::init(const setup_file_t& config) {
	if (vfs::device == this) {
		synao_log("Error! This virtual file system already exists!\n");
		return false;
	} else if (vfs::device != nullptr) {
		synao_log("Error! Another virtual filesystem already exists!\n");
		return false;
	}
	vfs::device = this;
	config.get("Setup", "Language", vfs::device->language);
	if (!vfs::try_language(vfs::device->language)) {
		synao_log("Error! Couldn't load first language: {}\n", vfs::device->language);
		return false;
	}
	vfs::device->thread_pool = std::make_unique<thread_pool_t>(kTotalThreads);
	if (vfs::device->thread_pool == nullptr) {
		synao_log("Error! Couldn't create thread pool!\n");
		return false;
	}
	vfs::device->personal = vfs::personal_directory();
	if (vfs::device->personal.empty()) {
		synao_log("Error! Couldn't find directory to store persistent data!\n");
		return false;
	}
	synao_log("Virtual filesystem initialized.\n");
	return true;
}

bool vfs::mount(const std::string& directory, bool_t print) {
	static const byte_t* kDirList[] = {
		kEventPath, kFieldPath,
		kFontPath, kI18NPath,
		kImagePath, kNoisePath,
		kPalettePath, kSpritePath,
		kTileKeyPath, kTunePath
	};
	if (!vfs::directory_exists(directory, print)) {
		return false;
	}
#ifndef LEVIATHAN_TOOLCHAIN_APPLECLANG
	std::error_code code;
	std::filesystem::current_path(directory, code);
	if (code) {
		synao_log("Failed to set working directory to \"{}\"!\n", directory);
		return false;
	}
#else
	sint_t result = chdir(directory.c_str());
	if (result != 0) {
		synao_log("Failed to set working directory to \"{}\"!\n", directory);
		return false;
	}
#endif
	bool success = true;
	for (arch_t it = 0; it < SYNAO_SIZEOF_ARRAY(kDirList); ++it) {
		if (!vfs::directory_exists(kDirList[it], print)) {
			success = false;
		}
	}
	return success;
}

bool vfs::directory_exists(const std::string& name, bool_t print) {
#ifndef LEVIATHAN_TOOLCHAIN_APPLECLANG
	if (!std::filesystem::exists(name) or !std::filesystem::is_directory(name)) {
		if (print) {
			synao_log("\"{}\" isn't a valid directory!\n", name);
		}
		return false;
	}
#else
	struct stat sb;
	if (stat(name.c_str(), &sb) != 0 or S_ISDIR(sb.st_mode) == 0) {
		if (print) {
			synao_log("\"{}\" isn't a valid directory!\n", name);
		}
		return false;
	}
#endif
	return true;
}

bool vfs::file_exists(const std::string& name, bool_t print) {
#ifndef LEVIATHAN_TOOLCHAIN_APPLECLANG
	if (!std::filesystem::exists(name) or !std::filesystem::is_regular_file(name)) {
		if (print) {
			synao_log("\"{}\" isn't a valid file!\n", name);
		}
		return false;
	}
#else
	struct stat sb;
	if (stat(name.c_str(), &sb) != 0 or S_ISREG(sb.st_mode) == 0) {
		if (print) {
			synao_log("\"{}\" isn't a valid file!\n", name);
		}
		return false;
	}
#endif
	return true;
}

bool vfs::create_directory(const std::string& name) {
	if (vfs::directory_exists(name)) {
		return true;
	}
#ifndef LEVIATHAN_TOOLCHAIN_APPLECLANG
	if (!std::filesystem::create_directory(name)) {
		synao_log("Failed to create file at: \"{}\"\n", name);
		return false;
	}
#else
	if (mkdir(name.c_str(), 0755) != 0) {
		synao_log("Failed to create file at: \"{}\"\n", name);
		return false;
	}
#endif
	return true;
}

bool vfs::create_recording(const std::string& path, const std::vector<uint16_t>& buffer, sint64_t seed) {
	std::ofstream ofs(path, std::ios::binary);
	if (ofs.is_open()) {
		arch_t length = buffer.size() * sizeof(uint16_t);
		ofs.write(reinterpret_cast<const byte_t*>(&seed), sizeof(sint64_t));
		ofs.write(reinterpret_cast<const byte_t*>(buffer.data()), length);
		return true;
	}
	synao_log("Failed to write file: {}!\n", path);
	return false;
}

std::string vfs::working_directory() {
#ifndef LEVIATHAN_TOOLCHAIN_APPLECLANG
	std::error_code code;
	auto path = std::filesystem::current_path(code);
	if (code) {
		synao_log("Failed get working directory!\n");
		return std::string();
	}
	return path.string();
#else
	byte_t buffer[1024];
	if (getcwd(buffer, sizeof(buffer)) == nullptr) {
		synao_log("Failed get working directory!\n");
		return std::string();
	}
	return std::string(buffer);
#endif
}

std::string vfs::executable_directory() {
	byte_t* path = SDL_GetBasePath();
	if (path == nullptr) {
		return std::string();
	}
	std::string result = path;
	SDL_free(path);
	return result;
}

std::string vfs::personal_directory() {
	byte_t* path = SDL_GetPrefPath(kOrgName, kAppName);
	if (path == nullptr) {
		return std::string();
	}
	std::string result = path;
	SDL_free(path);
	return result;
}

std::string vfs::resource_path(vfs_resource_path_t path) {
	switch (path) {
	case vfs_resource_path_t::Event:
		return kEventPath;
	case vfs_resource_path_t::Field:
		return kFieldPath;
	case vfs_resource_path_t::Font:
		return kFontPath;
	case vfs_resource_path_t::I18N:
		return kI18NPath;
	case vfs_resource_path_t::Image:
		return kImagePath;
	case vfs_resource_path_t::Init:
		if (vfs::device != nullptr) {
			return vfs::device->personal + kInitExt;
		}
		return vfs::personal_directory() + kInitExt;
	case vfs_resource_path_t::Noise:
		return kNoisePath;
	case vfs_resource_path_t::Palette:
		return kPalettePath;
	case vfs_resource_path_t::Save:
		if (vfs::device != nullptr) {
			return vfs::device->personal + kSaveExt;
		}
		return vfs::personal_directory() + kSaveExt;
	case vfs_resource_path_t::Sprite:
		return kSpritePath;
	case vfs_resource_path_t::TileKey:
		return kTileKeyPath;
	case vfs_resource_path_t::Tune:
		return kTunePath;
	default:
		break;
	}
	return std::string();
}

std::vector<std::string> vfs::file_list(const std::string& path) {
	std::vector<std::string> result;
#ifndef LEVIATHAN_TOOLCHAIN_APPLECLANG
	for (auto&& file : std::filesystem::directory_iterator(path)) {
		if (!file.is_directory()) {
			const std::string fname = file.path().filename().string();
			const std::string fstrn = fname.substr(0, fname.find_last_of("."));
			result.push_back(fstrn);
		}
	}
#else
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(path.c_str())) != nullptr) {
		while ((ent = readdir(dir)) != nullptr) {
			const std::string file = ent->d_name;
			const std::string name = file.substr(0, file.find_last_of("."));
			if (name.size() > 1) {
				result.push_back(name);
			}
		}
		closedir(dir);
	}
#endif
	return result;
}

std::string vfs::string_buffer(const std::string& path) {
	std::ifstream ifs(path, std::ios::binary);
	if (ifs.is_open()) {
		ifs.seekg(0, std::ios_base::end);
		arch_t length = static_cast<arch_t>(ifs.tellg());
		if (length > 0) {
			ifs.seekg(0, std::ios_base::beg);
			std::string buffer;
			buffer.resize(length);
			ifs.read(reinterpret_cast<byte_t*>(buffer.data()), length);
			return buffer;
		}
	}
	synao_log("Failed to open file: {}!\n", path);
	return std::string();
}

std::vector<byte_t> vfs::byte_buffer(const std::string& path) {
	std::ifstream ifs(path, std::ios::binary);
	if (ifs.is_open()) {
		ifs.seekg(0, std::ios_base::end);
		arch_t length = static_cast<arch_t>(ifs.tellg());
		if (length > 0) {
			ifs.seekg(0, std::ios_base::beg);
			std::vector<byte_t> buffer;
			buffer.resize(length);
			ifs.read(reinterpret_cast<byte_t*>(buffer.data()), length);
			return buffer;
		}
	}
	synao_log("Failed to open file: {}!\n", path);
	return std::vector<byte_t>();
}

std::vector<uint_t> vfs::uint32_buffer(const std::string& path) {
	std::ifstream ifs(path, std::ios::binary);
	if (ifs.is_open()) {
		ifs.seekg(0, std::ios_base::end);
		arch_t length = static_cast<arch_t>(ifs.tellg());
		if (length > 0) {
			ifs.seekg(0, std::ios_base::beg);
			std::vector<uint_t> buffer;
			buffer.resize(length / sizeof(uint_t));
			ifs.read(reinterpret_cast<byte_t*>(buffer.data()), length);
			return buffer;
		}
	}
	synao_log("Failed to open file: {}!\n", path);
	return std::vector<uint_t>();
}

bool vfs::record_buffer(const std::string& path, std::vector<uint16_t>& buffer, sint64_t& seed) {
	std::ifstream ifs(path, std::ios::binary);
	if (ifs.is_open()) {
		ifs.seekg(0, std::ios::end);
		arch_t length = static_cast<arch_t>(ifs.tellg());
		if (length > 0 and ((length / sizeof(uint16_t)) > sizeof(sint64_t))) {
			ifs.seekg(0, std::ios::beg);
			buffer.resize(
				(length / sizeof(uint16_t)) -
				sizeof(sint64_t)
			);
			ifs.read(reinterpret_cast<byte_t*>(&seed), sizeof(sint64_t));
			ifs.read(reinterpret_cast<byte_t*>(buffer.data()), length);
			return true;
		}
	}
	synao_log("Failed to open file: {}!\n", path);
	return false;
}

std::string vfs::i18n_find(const std::string& segment, arch_t index) {
	if (vfs::device == nullptr) {
		return std::string();
	}
	auto it = vfs::device->i18n.find(segment);
	if (it == vfs::device->i18n.end()) {
		return std::string();
	}
	std::string result;
	if (index < it->second.size()) {
		result = it->second[index];
	}
	return result;
}

std::string vfs::i18n_find(const std::string& segment, arch_t first, arch_t last) {
	if (vfs::device == nullptr) {
		return std::string();
	}
	auto it = vfs::device->i18n.find(segment);
	if (it == vfs::device->i18n.end()) {
		return std::string();
	}
	std::string result;
	if (first < it->second.size() and last < it->second.size()) {
		for (arch_t index = first; index <= last; ++index) {
			result += it->second[index];
		}
	}
	return result;
}

arch_t vfs::i18n_size(const std::string& segment) {
	if (vfs::device == nullptr) {
		return 0;
	}
	auto it = vfs::device->i18n.find(segment);
	if (it == vfs::device->i18n.end()) {
		return 0;
	}
	return it->second.size();
}

bool vfs::try_language(const std::string& language) {
	if (vfs::device == nullptr) {
		return false;
	}
	const std::string full_path = kI18NPath + language + ".json";
	std::unordered_map<std::string, std::vector<std::string> > i18n;
	std::ifstream ifs(full_path, std::ios::binary);
	if (ifs.is_open()) {
		nlohmann::json file = nlohmann::json::parse(ifs);
		for (auto it = file.begin(); it != file.end(); ++it) {
			std::vector<std::string>& vec = i18n[it.key()];
			for (auto&& s : it.value()) {
				vec.push_back(s.get<std::string>());
			}
		}
		vfs::device->language = language;
		vfs::device->i18n = std::move(i18n);
#if defined(LEVIATHAN_EXECUTABLE_NAOMI)
		vfs::device->fonts.clear();
#endif
		return true;
	}
	synao_log("Error! Couldn't load language file: {}\n", full_path);
	return false;
}

const texture_t* vfs::texture(const std::vector<std::string>& names, const std::string& directory) {
	if (vfs::device == nullptr) {
		return nullptr;
	}
	if (names.size() == 0 or names.size() > 4) {
		return nullptr;
	}
	// auto it = vfs::device->textures.find(names[0]);
	auto it = vfs::device->search_safely(names[0], vfs::device->textures);
	if (it == vfs::device->textures.end()) {
		texture_t& ref = vfs::device->emplace_safely(names[0], vfs::device->textures);
		auto generate_full_paths = [&directory](std::vector<std::string> names) {
			for (auto&& name : names) {
				name = directory + name + ".png";
			}
			return names;
		};
		ref.load(
			std::invoke(generate_full_paths, names),
			pixel_format_t::R8G8B8A8,
			*vfs::device->thread_pool
		);
		return &ref;
	}
	return &it->second;
}

const texture_t* vfs::texture(const std::vector<std::string>& names) {
	return vfs::texture(names, kImagePath);
}

const texture_t* vfs::texture(const std::string& name) {
	const std::vector<std::string> names = { name };
	return vfs::texture(names);
}

const palette_t* vfs::palette(const std::string& name, const std::string& directory) {
	if (vfs::device == nullptr) {
		return nullptr;
	}
	// auto it = vfs::device->palettes.find(name);
	auto it = vfs::device->search_safely(name, vfs::device->palettes);
	if (it == vfs::device->palettes.end()) {
		palette_t& ref = vfs::device->emplace_safely(name, vfs::device->palettes);
		ref.load(
			directory + name + ".png",
			pixel_format_t::R2G2B2A2,
			*vfs::device->thread_pool
		);
		return &ref;
	}
	return &it->second;
}

const palette_t* vfs::palette(const std::string& name) {
	return vfs::palette(name, kPalettePath);
}

const shader_t* vfs::shader(const std::string& name, const std::string& source, shader_stage_t stage) {
	if (vfs::device == nullptr) {
		return nullptr;
	}
	auto it = vfs::device->shaders.find(name);
	if (it == vfs::device->shaders.end()) {
		shader_t& ref = vfs::device->shaders[name];
		if (!ref.from(source, stage)) {
			synao_log("Failed to create shader from {}!\n", name);
		}
		return &ref;
	} else if (!it->second.matches(stage)) {
		synao_log("Found shader \"{}\" should have different stage!\n", name);
		return nullptr;
	}
	synao_log("Tried to create shader twice from source named {}!\n", name);
	return &it->second;
}

#if defined(LEVIATHAN_EXECUTABLE_NAOMI)

std::string vfs::event_path(const std::string& name, rec_loading_t flags) {
	if (vfs::device == nullptr) {
		synao_log("Couldn't find path for event: {}!\n", name);
		return std::string();
	}
	if (flags & rec_loading_t::Global) {
		return kEventPath + name + ".cc";
	}
	return kEventPath + vfs::device->language + '/' + name + ".cc";
}

const noise_t* vfs::noise(const std::string& name) {
	const table_entry_t entry = table_entry_t(name.c_str());
	return vfs::noise(entry);
}

const noise_t* vfs::noise(const table_entry_t& entry) {
	if (vfs::device == nullptr) {
		return nullptr;
	}
	// auto it = vfs::device->noises.find(entry.hash);
	auto it = vfs::device->search_safely(entry.hash, vfs::device->noises);
	if (it == vfs::device->noises.end()) {
		noise_t& ref = vfs::device->emplace_safely(entry.hash, vfs::device->noises);
		ref.load(kNoisePath + std::string(entry.name) + ".wav", *vfs::device->thread_pool);
		return &ref;
	}
	return &it->second;
}

const animation_t* vfs::animation(const table_entry_t& entry) {
	if (vfs::device == nullptr) {
		return nullptr;
	}
	// auto it = vfs::device->animations.find(entry.hash);
	auto it = vfs::device->search_safely(entry.hash, vfs::device->animations);
	if (it == vfs::device->animations.end()) {
		animation_t& ref = vfs::device->emplace_safely(entry.hash, vfs::device->animations);
		ref.load(kSpritePath + std::string(entry.name) + ".cfg", *vfs::device->thread_pool);
		return &ref;
	}
	return &it->second;
}

const animation_t* vfs::animation(const std::string& name) {
	const table_entry_t entry = table_entry_t(name.c_str());
	return vfs::animation(entry);
}

const font_t* vfs::font(const std::string& name) {
	if (vfs::device == nullptr) {
		return nullptr;
	}
	auto it = vfs::device->fonts.find(name);
	if (it == vfs::device->fonts.end()) {
		font_t& ref = vfs::device->fonts[name];
		ref.load(kFontPath, name + ".fnt");
		return &ref;
	}
	return &it->second;
}

const font_t* vfs::font(arch_t index) {
	if (vfs::device == nullptr) {
		return nullptr;
	}
	auto& i18n_fonts = vfs::device->i18n["Fonts"];
	if (index < i18n_fonts.size()) {
		return vfs::font(i18n_fonts[index]);
	}
	return nullptr;
}

const font_t* vfs::debug_font() {
	return vfs::font(kDebugFontIndex);
}

#endif
