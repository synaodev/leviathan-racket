#include "./vfs.hpp"

#include "../utility/logger.hpp"
#include "../utility/setup-file.hpp"

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <streambuf>

#if defined(LEVIATHAN_TOOLCHAIN_APPLECLANG)
	#include <ghc/filesystem.hpp>
	namespace fs = ghc::filesystem;
#else
	#include <filesystem>
	namespace fs = std::filesystem;
#endif

#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <SDL2/SDL_filesystem.h>

static constexpr arch_t kTotalThreads 	= 4;
static constexpr arch_t kDebugFontIndex = 4;
static constexpr byte_t kOrganization[] = "studio-synao";
static constexpr byte_t kApplication[] 	= "leviathan";
static constexpr byte_t kLanguage[] 	= "english";
static constexpr byte_t kEventEntry[]	= "Events";

#define kDATA_ROUTE "data/"
static constexpr byte_t kDataRoute[] 	= kDATA_ROUTE;
static constexpr byte_t kInitRoute[] 	= "init/";
static constexpr byte_t kSaveRoute[]	= "save/";

static constexpr byte_t kEventPath[]	= kDATA_ROUTE "event/";
static constexpr byte_t kFieldPath[]	= kDATA_ROUTE "field/";
static constexpr byte_t kFontPath[]		= kDATA_ROUTE "font/";
static constexpr byte_t kI18NPath[]		= kDATA_ROUTE "i18n/";
static constexpr byte_t kImagePath[]	= kDATA_ROUTE "image/";
static constexpr byte_t kNoisePath[]	= kDATA_ROUTE "noise/";
static constexpr byte_t kPalettePath[]	= kDATA_ROUTE "palette/";
static constexpr byte_t kSpritePath[]	= kDATA_ROUTE "sprite/";
static constexpr byte_t kTileKeyPath[]	= kDATA_ROUTE "tilekey/";
static constexpr byte_t kTunePath[]		= kDATA_ROUTE "tune/";

/*
	Not a fully featured virtual filesystem, obviously. Here's the layout:

	data/
		event/
			[lang]/
				*.cc
			boot.cc
		field/
			*.tmx
		font/
			*.fnt
			*.png
			*.bmfc
		i18n/
			*.json
		image/
			icon.png
			*.png
		noise/
			*.wav
		palette/
			*.png
		sprite/
			*.json
		tilekey/
			*.atr
		tune/
			*.ptcop
			*.pttune
	init/
		boot.cfg
		*.macro
	save/
		*_check.bin
		*_check.cfg
		*_prog.bin
		*_prog.cfg

	The "data" directory should be in the working directory (i.e. the executable's directory).
	The "init" and "save" directories should be in the directory returned by SDL_PrefPath().
*/

vfs_t* vfs_t::device { nullptr };

vfs_t::~vfs_t() {
	if (vfs_t::device) {
		if (vfs_t::device == this) {
			vfs_t::device = nullptr;
		} else {
			synao_log("Error! There should not be more than one virtual filesystem!\n");
		}
	}
}

bool vfs_t::init(const setup_file_t& config) {
	if (vfs_t::device == this) {
		synao_log("Error! This virtual file system already exists!\n");
		return false;
	} else if (vfs_t::device) {
		synao_log("Error! Another virtual filesystem already exists!\n");
		return false;
	}
	vfs_t::device = this;

	// Setup Language
	std::string language = kLanguage;
	config.get("Setup", "Language", language);
	if (!vfs_t::try_language(language)) {
		synao_log("Error! Couldn't load first language: {}\n", language);
		return false;
	}

	// Setup Thread Pool
	if (!vfs_t::device->thread_pool.init(kTotalThreads)) {
		synao_log("Error! Couldn't create thread pool!\n");
		return false;
	}

	// Setup Filesystem
	vfs_t::device->personal = vfs_t::personal_directory();
	if (vfs_t::device->personal.empty()) {
		synao_log("Error! Couldn't find directory to store persistent data!\n");
		return false;
	}
	synao_log("Virtual filesystem initialized.\n");
	return true;
}

bool vfs_t::set_sampler_allocator(sampler_allocator_t* sampler_allocator) {
	if (vfs_t::device != this) {
		synao_log("Error! This virtual filesystem isn't ready to setup the sampler allocator!\n");
		return false;
	}
	if (this->sampler_allocator or this->sampler_allocator == sampler_allocator) {
		synao_log("Error! This virtual filesystem aleady has a sampler allocator!\n");
		return false;
	}
	this->sampler_allocator = sampler_allocator;
	return true;
}

bool vfs_t::mount(const std::string& directory, bool_t print) {
	static const byte_t* kDirList[] = {
		kEventPath, kFieldPath,
		kFontPath, kI18NPath,
		kImagePath, kNoisePath,
		kPalettePath, kSpritePath,
		kTileKeyPath, kTunePath
	};
	if (!vfs_t::directory_exists(directory, print)) {
		return false;
	}
	std::error_code code;
	fs::current_path(directory, code);
	if (code) {
		synao_log("Failed to set working directory to \"{}\"!\n", directory);
		return false;
	}
	bool success = true;
	for (arch_t it = 0; it < (sizeof(kDirList) / sizeof(kDirList[0])); ++it) {
		if (!vfs_t::directory_exists(kDirList[it], print)) {
			success = false;
		}
	}
	return success;
}

bool vfs_t::directory_exists(const std::string& name, bool_t print) {
	if (!fs::exists(name) or !fs::is_directory(name)) {
		if (print) {
			synao_log("\"{}\" isn't a valid directory!\n", name);
		}
		return false;
	}
	return true;
}

bool vfs_t::file_exists(const std::string& name, bool_t print) {
	if (!fs::exists(name) or !fs::is_regular_file(name)) {
		if (print) {
			synao_log("\"{}\" isn't a valid file!\n", name);
		}
		return false;
	}
	return true;
}

bool vfs_t::create_directory(const std::string& name) {
	if (vfs_t::directory_exists(name)) {
		return true;
	}
	if (!fs::create_directory(name)) {
		synao_log("Failed to create file at: \"{}\"\n", name);
		return false;
	}
	return true;
}

bool vfs_t::create_recording(const std::string& path, const std::vector<uint16_t>& buffer, sint64_t seed) {
	std::ofstream ofs { path, std::ios::binary };
	if (ofs.is_open()) {
		arch_t length = buffer.size() * sizeof(uint16_t);
		ofs.write(reinterpret_cast<const byte_t*>(&seed), sizeof(sint64_t));
		ofs.write(reinterpret_cast<const byte_t*>(buffer.data()), length);
		return true;
	}
	synao_log("Failed to write file: {}!\n", path);
	return false;
}

std::string vfs_t::working_directory() {
	std::error_code code;
	auto path = fs::current_path(code);
	if (code) {
		synao_log("Failed get working directory!\n");
		return {};
	}
	return path.string();
}

std::string vfs_t::executable_directory() {
	byte_t* path = SDL_GetBasePath();
	if (!path) {
		return {};
	}
	std::string result = path;
	SDL_free(path);
	return result;
}

std::string vfs_t::personal_directory() {
	byte_t* path = SDL_GetPrefPath(kOrganization, kApplication);
	if (!path) {
		return {};
	}
	std::string result = path;
	SDL_free(path);
	return result;
}

std::string vfs_t::resource_path(vfs_resource_path_t path) {
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
		if (vfs_t::device) {
			return vfs_t::device->personal + kInitRoute;
		}
		return vfs_t::personal_directory() + kInitRoute;
	case vfs_resource_path_t::Noise:
		return kNoisePath;
	case vfs_resource_path_t::Palette:
		return kPalettePath;
	case vfs_resource_path_t::Save:
		if (vfs_t::device) {
			return vfs_t::device->personal + kSaveRoute;
		}
		return vfs_t::personal_directory() + kSaveRoute;
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

std::vector<std::string> vfs_t::file_list(const std::string& path) {
	std::vector<std::string> result;
	for (auto&& file : fs::directory_iterator(path)) {
		if (!file.is_directory()) {
			const std::string full_name = file.path().filename().string();
			const std::string short_name = full_name.substr(0, full_name.find_last_of("."));
			result.push_back(short_name);
		}
	}
	return result;
}

std::string vfs_t::string_buffer(const std::string& path) {
	std::ifstream ifs { path, std::ios::binary };
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
	return {};
}

std::vector<byte_t> vfs_t::byte_buffer(const std::string& path) {
	std::ifstream ifs { path, std::ios::binary };
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
	return {};
}

std::vector<uint_t> vfs_t::uint32_buffer(const std::string& path) {
	std::ifstream ifs { path, std::ios::binary };
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
	return {};
}

bool vfs_t::record_buffer(const std::string& path, std::vector<uint16_t>& buffer, sint64_t& seed) {
	std::ifstream ifs { path, std::ios::binary };
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

std::string vfs_t::i18n_find(const std::string& segment, arch_t index) {
	if (!vfs_t::device) {
		return {};
	}
	auto it = vfs_t::device->i18n.find(segment);
	if (it == vfs_t::device->i18n.end()) {
		return {};
	}
	fmt::memory_buffer result;
	if (index < it->second.size()) {
		result.append(it->second[index]);
	}
	return fmt::to_string(result);
}

std::string vfs_t::i18n_find(const std::string& segment, arch_t first, arch_t last) {
	if (!vfs_t::device) {
		return {};
	}
	auto it = vfs_t::device->i18n.find(segment);
	if (it == vfs_t::device->i18n.end()) {
		return {};
	}
	fmt::memory_buffer result;
	if (first < it->second.size() and last < it->second.size()) {
		for (arch_t index = first; index <= last; ++index) {
			result.append(it->second[index]);
		}
	}
	return fmt::to_string(result);
}

arch_t vfs_t::i18n_size(const std::string& segment) {
	if (!vfs_t::device) {
		return 0;
	}
	auto it = vfs_t::device->i18n.find(segment);
	if (it == vfs_t::device->i18n.end()) {
		return 0;
	}
	return it->second.size();
}

bool vfs_t::try_language(const std::string& language) {
	if (!vfs_t::device) {
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
		vfs_t::device->language = language;
		vfs_t::device->i18n = std::move(i18n);
		vfs_t::device->fonts.clear();
		vfs_t::device->atlases.clear();
		return true;
	}
	synao_log("Error! Couldn't load language file: {}\n", full_path);
	return false;
}

const texture_t* vfs_t::texture(const std::string& name) {
	if (!vfs_t::device) {
		return nullptr;
	}
	if (!vfs_t::device->sampler_allocator) {
		return nullptr;
	}
	auto it = vfs_t::device->search_safely(name, vfs_t::device->textures);
	if (it == vfs_t::device->textures.end()) {
		texture_t& ref = vfs_t::device->emplace_safely(name, vfs_t::device->textures);
		ref.load(
			kImagePath + name + ".png",
			vfs_t::device->sampler_allocator,
			vfs_t::device->thread_pool
		);
		return &ref;
	}
	return &it->second;
}

const palette_t* vfs_t::palette(const std::string& name) {
	if (!vfs_t::device) {
		return nullptr;
	}
	if (!vfs_t::device->sampler_allocator) {
		return nullptr;
	}
	auto it = vfs_t::device->search_safely(name, vfs_t::device->palettes);
	if (it == vfs_t::device->palettes.end()) {
		palette_t& ref = vfs_t::device->emplace_safely(name, vfs_t::device->palettes);
		ref.load(
			kPalettePath + name + ".png",
			vfs_t::device->sampler_allocator,
			vfs_t::device->thread_pool
		);
		return &ref;
	}
	return &it->second;
}

const atlas_t* vfs_t::atlas(const std::string& name) {
	if (!vfs_t::device) {
		return nullptr;
	}
	if (!vfs_t::device->sampler_allocator) {
		return nullptr;
	}
	auto it = vfs_t::device->search_safely(name, vfs_t::device->atlases);
	if (it == vfs_t::device->atlases.end()) {
		atlas_t& ref = vfs_t::device->emplace_safely(name, vfs_t::device->atlases);
		ref.load(
			kFontPath + name + ".png",
			vfs_t::device->sampler_allocator,
			vfs_t::device->thread_pool
		);
		return &ref;
	}
	return &it->second;
}

const shader_t* vfs_t::shader(const std::string& name, const std::string& source, shader_stage_t stage) {
	if (!vfs_t::device) {
		return nullptr;
	}
	auto it = vfs_t::device->shaders.find(name);
	if (it == vfs_t::device->shaders.end()) {
		shader_t& ref = vfs_t::device->shaders[name];
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

std::string vfs_t::event_path(const std::string& name, event_loading_t flags) {
	if (!vfs_t::device) {
		synao_log("Couldn't find path for event: {}!\n", name);
		return std::string();
	}
	if (flags & event_loading_t::Global) {
		return kEventPath + name + ".as";
	}
	return kEventPath + vfs_t::i18n_find(kEventEntry, 0) + '/' + name + ".as";
}

const noise_t* vfs_t::noise(const std::string& name) {
	const entt::hashed_string entry{name.c_str()};
	return vfs_t::noise(entry);
}

const noise_t* vfs_t::noise(const entt::hashed_string& entry) {
	if (!vfs_t::device) {
		return nullptr;
	}
	auto it = vfs_t::device->search_safely(entry.value(), vfs_t::device->noises);
	if (it == vfs_t::device->noises.end()) {
		noise_t& ref = vfs_t::device->emplace_safely(entry.value(), vfs_t::device->noises);
		ref.load(kNoisePath + std::string(entry.data()) + ".wav", vfs_t::device->thread_pool);
		return &ref;
	}
	return &it->second;
}

const animation_t* vfs_t::animation(const entt::hashed_string& entry) {
	if (!vfs_t::device) {
		return nullptr;
	}
	auto it = vfs_t::device->search_safely(entry.value(), vfs_t::device->animations);
	if (it == vfs_t::device->animations.end()) {
		animation_t& ref = vfs_t::device->emplace_safely(entry.value(), vfs_t::device->animations);
		ref.load(kSpritePath + std::string(entry.data()) + ".json", vfs_t::device->thread_pool);
		return &ref;
	}
	return &it->second;
}

const animation_t* vfs_t::animation(const std::string& name) {
	const entt::hashed_string entry{ name.c_str() };
	return vfs_t::animation(entry);
}

const font_t* vfs_t::font(const std::string& name) {
	if (!vfs_t::device) {
		return nullptr;
	}
	auto it = vfs_t::device->fonts.find(name);
	if (it == vfs_t::device->fonts.end()) {
		font_t& ref = vfs_t::device->fonts[name];
		ref.load(kFontPath, name + ".fnt");
		return &ref;
	}
	return &it->second;
}

const font_t* vfs_t::font(arch_t index) {
	if (!vfs_t::device) {
		return nullptr;
	}
	auto& i18n_fonts = vfs_t::device->i18n["Fonts"];
	if (index < i18n_fonts.size()) {
		return vfs_t::font(i18n_fonts[index]);
	}
	return nullptr;
}

const font_t* vfs_t::debug_font() {
	return vfs_t::font(kDebugFontIndex);
}
