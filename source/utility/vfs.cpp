#include "./vfs.hpp"
#include "./logger.hpp"
#include "./setup_file.hpp"
#include "./thread_pool.hpp"
#include "../resource/tbl_entry.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <nlohmann/json.hpp>

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
	#include <sys/sysctl.h>
#endif

#ifndef LEVIATHAN_TOOLCHAIN_APPLECLANG
	#include <filesystem>
#endif

#define SYNAO_SIZEOF_ARRAY(ARR) (sizeof( ARR ) / sizeof( ARR [0] ))

static const byte_t kEventPath[]	= "./data/event/";
static const byte_t kFieldPath[]	= "./data/field/";
static const byte_t kFontPath[]		= "./data/font/";
static const byte_t kI18NPath[]		= "./data/i18n/";
static const byte_t kImagePath[]	= "./data/image/";
static const byte_t kInitPath[]		= "./data/init/";
static const byte_t kNoisePath[]	= "./data/noise/";
static const byte_t kPalettePath[]	= "./data/palette/";
static const byte_t kSavePath[]		= "./data/save/";
static const byte_t kSpritePath[]	= "./data/sprite/";
static const byte_t kTileKeyPath[]	= "./data/tilekey/";
static const byte_t kTunePath[]		= "./data/tune/";

static constexpr byte_t kDefaultLang[] 	= "english";
static constexpr arch_t kTotalThreads 	= 4;
static constexpr arch_t kDebugFontIndex = 4;

vfs_t::vfs_t() :
	thread_pool(),
	storage_mutex(),
	language(kDefaultLang),
	i18n(),
	noises(),
	textures(),
	palettes(),
	shaders(),
	fonts(),
	animations()
{

}

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
	config.get("Setup", "Language", language);
	if (!vfs::try_language(language)) {
		synao_log("Error! Could not load first language: %s\n", language.c_str());
		return false;
	}
	vfs::device->thread_pool = std::make_unique<thread_pool_t>(kTotalThreads);
	if (vfs::device->thread_pool == nullptr) {
		synao_log("Error! Couldn't create thread pool!\n");
		return false;
	}
	synao_log("Virtual filesystem initialized.\n");
	return true;
}

// Copied from SFML
static std::string::const_iterator decode(std::string::const_iterator begin, std::string::const_iterator end, uint_t& output, uint_t replacement = 0) {
	static const sint_t trailing[256] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5
	};

	static const uint_t offsets[6] = {
		0x00000000, 0x00003080, 0x000E2080, 0x03C82080, 0xFA082080, 0x82082080
	};

	sint_t trailingBytes = trailing[static_cast<uint8_t>(*begin)];

	if (begin + trailingBytes < end) {
		output = 0;
		switch (trailingBytes) {
			case 5: output += static_cast<uint8_t>(*begin++); output <<= 6;
			case 4: output += static_cast<uint8_t>(*begin++); output <<= 6;
			case 3: output += static_cast<uint8_t>(*begin++); output <<= 6;
			case 2: output += static_cast<uint8_t>(*begin++); output <<= 6;
			case 1: output += static_cast<uint8_t>(*begin++); output <<= 6;
			case 0: output += static_cast<uint8_t>(*begin++);
		}
		output -= offsets[trailingBytes];
	} else {
		begin = end;
		output = replacement;
	}

	return begin;
}

std::back_insert_iterator<std::u32string> vfs::to_utf32(
	std::string::const_iterator begin,
	std::string::const_iterator end,
	std::back_insert_iterator<std::u32string> output) {
	while (begin < end) {
		uint_t point;
		begin = ::decode(begin, end, point);
		*output++ = point;
	}
	return output;
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
		synao_log("Failed to set working directory to \"%s\"!\n", directory.c_str());
		return false;
	}
#else
	sint_t result = chdir(directory.c_str());
	if (result != 0) {
		synao_log("Failed to set working directory to \"%s\"!\n", directory.c_str());
		return false;
	}
#endif
	bool problem = true;
	for (arch_t it = 0; it < SYNAO_SIZEOF_ARRAY(kDirList); ++it) {
		if (!vfs::directory_exists(kDirList[it], print)) {
			problem = false;
		}
	}
	return problem;
}

bool vfs::directory_exists(const std::string& name, bool_t print) {
#ifndef LEVIATHAN_TOOLCHAIN_APPLECLANG
	if (!std::filesystem::exists(name) or !std::filesystem::is_directory(name)) {
		if (print) {
			synao_log("\"%s\" isn't a valid directory!\n", name.c_str());
		}
		return false;
	}
#else
	struct stat sb;
	if (stat(name.c_str(), &sb) != 0 or S_ISDIR(sb.st_mode) == 0) {
		if (print) {
			synao_log("\"%s\" isn't a valid directory!\n", name.c_str());
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
			synao_log("\"%s\" isn't a valid file!\n", name.c_str());
		}
		return false;
	}
#else
	struct stat sb;
	if (stat(name.c_str(), &sb) != 0 or S_ISREG(sb.st_mode) == 0) {
		if (print) {
			synao_log("\"%s\" isn't a valid file!\n", name.c_str());
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
	std::error_code code;
	if (!std::filesystem::create_directory(name, code)) {
		synao_log("Failed to create file at: \"%s\"\n", name.c_str());
		return false;
	}
#else
	if (mkdir(name.c_str(), 0755) != 0) {
		synao_log("Failed to create file at: \"%s\"\n", name.c_str());
		return false;
	}
#endif
	return true;
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
	std::string result;
#if defined(LEVIATHAN_PLATFORM_WINDOWS)
	byte_t buffer[MAX_PATH];
	::GetModuleFileNameA(NULL, buffer, sizeof(buffer));
	if (strlen(buffer) > 0) {
		result = buffer;
	}
#elif defined(LEVIATHAN_PLATFORM_MACOS)
	byte_t buffer[PROC_PIDPATHINFO_MAXSIZE];
	pid_t pid = getpid();
	if (proc_pidpath(pid, buffer, sizeof(buffer)) > 0) {
		result = buffer;
	}
#elif defined(LEVIATHAN_PLATFORM_LINUX)
	byte_t buffer[PATH_MAX + 1];
	if (readlink("/proc/self/exe", buffer, PATH_MAX) > 0) {
		result = buffer;
	}
#elif defined(LEVIATHAN_PLATFORM_FREEBSD) || defined(LEVIATHAN_PLATFORM_OPENBSD)
	byte_t buffer[1024];
	arch_t length = sizeof(buffer);
	sint_t params[4] = {
		CTL_KERN,
		KERN_PROC,
		KERN_PROC_PATHNAME,
		-1
	};
	if (sysctl(params, sizeof(params), buffer, &length, NULL, 0) != -1) {
		result = buffer;
	}
#elif defined(LEVIATHAN_PLATFORM_NETBSD)
	byte_t buffer[PATH_MAX + 1];
	if (readlink("/proc/curproc/exe"), buffer, PATH_MAX) > 0) {
		result = buffer;
	}
#elif defined(LEVIATHAN_PLATFORM_DRAGONFLY)
	byte_t buffer[PATH_MAX + 1];
	if (readlink("/proc/curproc/file"), buffer, PATH_MAX) > 0) {
		result = buffer;
	}
#endif
	arch_t position = result.find_last_of("\\/");
	if (position == std::string::npos) {
		synao_log("Failed get executable directory!\n");
		return std::string();
	}
	return result.substr(0, position);
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
		return kInitPath;
	case vfs_resource_path_t::Noise:
		return kNoisePath;
	case vfs_resource_path_t::Palette:
		return kPalettePath;
	case vfs_resource_path_t::Save:
		return kSavePath;
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
	std::ifstream ifs(path);
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
	synao_log("Failed to open file: %s!\n", path.c_str());
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
	synao_log("Failed to open file: %s!\n", path.c_str());
	return std::vector<byte_t>();
}

std::vector<sint_t> vfs::sint_buffer(const std::string& path) {
	std::ifstream ifs(path, std::ios::binary);
	if (ifs.is_open()) {
		ifs.seekg(0, std::ios_base::end);
		arch_t length = static_cast<arch_t>(ifs.tellg());
		if (length > 0) {
			ifs.seekg(0, std::ios_base::beg);
			std::vector<sint_t> buffer;
			buffer.resize(length / sizeof(sint_t));
			ifs.read(reinterpret_cast<byte_t*>(buffer.data()), length);
			return buffer;
		}
	}
	synao_log("Failed to open file: %s!\n", path.c_str());
	return std::vector<sint_t>();
}

std::string vfs::event_path(const std::string& name, rec_loading_t flags) {
	if (vfs::device == nullptr) {
		synao_log("Couldn't find path for event: %s!\n", name.c_str());
		return std::string();
	}
	if (flags & rec_loading_t::Global) {
		return kEventPath + name + ".cc";
	}
	return kEventPath + vfs::device->language + '/' + name + ".cc";
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
		vfs::device->fonts.clear();
		return true;
	}
	synao_log("Error! Couldn't load language file: %s\n", full_path.c_str());
	return false;
}

const noise_t* vfs::noise(const std::string& name) {
	const tbl_entry_t entry = tbl_entry_t(name.c_str());
	return vfs::noise(entry);
}

const noise_t* vfs::noise(const tbl_entry_t& entry) {
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

const animation_t* vfs::animation(const tbl_entry_t& entry) {
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
	const tbl_entry_t entry = tbl_entry_t(name.c_str());
	return vfs::animation(entry);
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
			synao_log("Failed to create shader from %s!\n", name.c_str());
		}
		return &ref;
	} else if (!it->second.matches(stage)) {
		synao_log("Found shader %s should have different stage!\n", name.c_str());
		return nullptr;
	}
	synao_log("Tried to create shader twice from source named %s!\n", name.c_str());
	return &it->second;
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
