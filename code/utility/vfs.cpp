#include "./vfs.hpp"
#include "./logger.hpp"
#include "./setup_file.hpp"
#include "./thread_pool.hpp"
#include "./json.hpp"

#include <fstream>
#include <sstream>
#include <streambuf>

#if !defined(__APPLE__) && !defined(__GNUC__)
#include <filesystem>
#else // __APPLE__ __GNUC__
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif // __APPLE__ __GNUC__

#define SYNAO_SIZEOF_ARRAY(ARR) (sizeof( ARR ) / sizeof( ARR [0]))

static const byte_t kEventPath[]	= "./event/";
static const byte_t kFieldPath[]	= "./field/";
static const byte_t kFontPath[]		= "./font/";
static const byte_t kI18NPath[]		= "./i18n/";
static const byte_t kImagePath[]	= "./image/";
static const byte_t kTileKeyPath[]	= "./tilekey/";
static const byte_t kNoisePath[]	= "./noise/";
static const byte_t kPalettePath[]	= "./palette/";
static const byte_t kPxtonePath[]	= "./pxtone/";
static const byte_t kSpritePath[]	= "./sprite/";

static constexpr byte_t kDefaultLang[] = "english";
static constexpr arch_t kTotalThreads  = 4;

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
			SYNAO_LOG("Error! There should not be more than one virtual filesystem!\n");
		}
	}
}

bool vfs_t::mount(const setup_file_t& config) {
	if (vfs::device == this) {
		SYNAO_LOG("Error! This virtual file system already exists!\n");
		return false;
	} else if (vfs::device != nullptr) {
		SYNAO_LOG("Error! Another virtual filesystem already exists!\n");
		return false;
	}
	vfs::device = this;
	config.get("Setup", "Language", language);
	if (!vfs::try_language(language)) {
		SYNAO_LOG("Error! Could not load first language: %s\n", language.c_str());
		return false;
	}
	vfs::device->thread_pool = std::make_unique<thread_pool_t>(kTotalThreads);
	if (vfs::device->thread_pool == nullptr) {
		SYNAO_LOG("Error! Couldn't create thread pool!\n");
		return false;
	}
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

bool vfs::directory_exists(const std::string& name) {
#if !defined(__APPLE__) && !defined(__GNUC__)
	if (!std::filesystem::exists(name)) {
		SYNAO_LOG("Cannot find \"%s\"!\n", name.c_str());
		return false;
	}
	if (!std::filesystem::is_directory(name)) {
		SYNAO_LOG("\"%s\" isn't a directory!\n", name.c_str());
		return false;
	}
#else // __APPLE__ __GNUC__
	struct stat sb;
	if (stat(name.c_str(), &sb) != 0) {
		SYNAO_LOG("Cannot find \"%s\"!\n", name.c_str());
		return false;
	}
	if (S_ISDIR(sb.st_mode) == 0) {
		SYNAO_LOG("\"%s\" isn't a directory!\n", name.c_str());
		return false;
	}
#endif // __APPLE__ __GNUC__
	return true;
}

bool vfs::create_directory(const std::string& name) {
	if (vfs::directory_exists(name)) {
		return true;
	}
#if !defined(__APPLE__) && !defined(__GNUC__)
	return std::filesystem::create_directory(name);
#else // __APPLE__ __GNUC__
	return mkdir(name.c_str(), 0755) == 0;
#endif // __APPLE__ __GNUC__
}

bool vfs::verify_structure() {
	const byte_t* kDirList[] = {
		kEventPath, kFieldPath, 
		kFontPath, kI18NPath, 
		kImagePath, kTileKeyPath,
		kNoisePath, kPalettePath, 
		kPxtonePath, kSpritePath
	};
	bool result = true;
	for (arch_t it = 0; it < SYNAO_SIZEOF_ARRAY(kDirList); ++it) {
		if (!vfs::directory_exists(kDirList[it])) {
			result = false;
		}
	}
	return result;
}

std::vector<std::string> vfs::file_list(const std::string& path) {
	std::vector<std::string> result;
#if !defined(__APPLE__) && !defined(__GNUC__)
	for (auto&& file : std::filesystem::directory_iterator(path)) {
		if (!file.is_directory()) {
			const std::string fname = file.path().filename().string();
			const std::string fstrn = fname.substr(0, fname.find_last_of("."));
			result.push_back(fstrn);
		}
	}
#else // __APPLE__ __GNUC__
	DIR* dir;
	struct dirent* ent;
	if ((dir = opendir(path.c_str())) != nullptr) {
		while ((ent = readdir(dir)) != nullptr) {
			const std::string fname = ent->d_name;
			const std::string fstrn = fname.substr(0, fname.find_last_of("."));
			if (fstrn.size() > 1) {
				result.push_back(fstrn);
			}
		}
		closedir(dir);
	}
#endif // __APPLE__ __GNUC__
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
	SYNAO_LOG("Failed to open file: %s!\n", path.c_str());
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
	SYNAO_LOG("Failed to open file: %s!\n", path.c_str());
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
	SYNAO_LOG("Failed to open file: %s!\n", path.c_str());
	return std::vector<sint_t>();
}

std::string vfs::event_path(const std::string& name, rec_loading_t flags) {
	if (vfs::device == nullptr) {
		SYNAO_LOG("Couldn't find path for event: %s!\n", name.c_str());
		return std::string();
	}
	if (flags & rec_loading_t::Global) {
		return kEventPath + name + ".as";
	}
	return kEventPath + vfs::device->language + '/' + name + ".as";
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
	SYNAO_LOG(
		"Error! Couldn't load language file: %s\n", 
		full_path.c_str()
	);
	return false;
}

std::string vfs::local_script(const std::string& name) {
	if (vfs::device == nullptr) {
		return std::string();
	}
	return kEventPath + vfs::device->language + '/' + name + ".as";
}

std::string vfs::global_script(const std::string& name) {
	if (vfs::device == nullptr) {
		return std::string();
	}
	return kEventPath + name + ".as";
}

const noise_t* vfs::noise(const std::string& name) {
	if (vfs::device == nullptr) {
		return nullptr;
	}
	auto it = vfs::device->noises.find(name);
	if (it == vfs::device->noises.end()) {
		noise_t& ref = vfs::device->allocate_safely(name, vfs::device->noises);
		const std::string full_path = kNoisePath + name + ".wav";
		if (!ref.load(full_path, *vfs::device->thread_pool)) {
			SYNAO_LOG("Failed to load noise from %s!\n", full_path.c_str());
		}
		return &ref;
	}
	return &it->second;
}

const texture_t* vfs::texture(const std::vector<std::string>& names, const std::string& directory) {
	if (vfs::device == nullptr) {
		return nullptr;
	}
	if (names.size() == 0 or names.size() > 4) {
		return nullptr;
	}
	auto it = vfs::device->textures.find(names[0]);
	if (it == vfs::device->textures.end()) {
		texture_t& ref = vfs::device->allocate_safely(names[0], vfs::device->textures);
		auto generate_full_paths = [&directory](std::vector<std::string> names) {
			for (auto&& name : names) {
				name = directory + name + ".png";
			}
			return names;
		};
		std::vector<std::string> full_paths = generate_full_paths(names);
		if (!ref.load(full_paths, pixel_format_t::R8G8B8A8, *vfs::device->thread_pool)) {
#ifdef SYNAO_DEBUG_BUILD
			std::for_each(full_paths.begin(), full_paths.end(), [](const std::string& full_path) {
				SYNAO_LOG("Failed to load texture from %s!\n", full_path.c_str());
			});
#endif // SYNAO_DEBUG_BUILD
		}
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
	auto it = vfs::device->palettes.find(name);
	if (it == vfs::device->palettes.end()) {
		palette_t& ref = vfs::device->allocate_safely(name, vfs::device->palettes);
		const std::string full_path = directory + name + ".png";
		if (!ref.load(full_path, pixel_format_t::R2G2B2A2, *vfs::device->thread_pool)) {
			SYNAO_LOG(
				"Failed to load palette from %s!\n", 
				full_path.c_str()
			);
		}
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
		shader_t& ref = vfs::device->allocate_safely(name, vfs::device->shaders);
		if (!ref.from(source, stage)) {
			SYNAO_LOG(
				"Failed to create shader from %s!\n", 
				name.c_str()
			);
		}
		return &ref;
	} else if (!it->second.matches(stage)) {
		SYNAO_LOG(
			"Found shader %s should have different stage!\n", 
			name.c_str()
		);
		return nullptr;
	}
	SYNAO_LOG(
		"Tried to create shader twice from source named %s!\n", 
		name.c_str()
	);
	return &it->second;
}

const font_t* vfs::font(const std::string& name) {
	if (vfs::device == nullptr) {
		return nullptr;
	}
	auto it = vfs::device->fonts.find(name);
	if (it == vfs::device->fonts.end()) {
		const std::string full_path = kFontPath + name + ".fnt";
		font_t& ref = vfs::device->allocate_safely(name, vfs::device->fonts);
		if (!ref.load(kFontPath, full_path)) {
			SYNAO_LOG(
				"Failed to load font from %s!\n", 
				full_path.c_str()
			);
		}
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

const animation_t* vfs::animation(const std::string& name) {
	if (vfs::device == nullptr) {
		return nullptr;
	}
	auto it = vfs::device->animations.find(name);
	if (it == vfs::device->animations.end()) {
		const std::string full_path = kSpritePath + name + ".cfg";
		animation_t& ref = vfs::device->allocate_safely(name, vfs::device->animations);
		if (!ref.load(full_path, *vfs::device->thread_pool)) {
			SYNAO_LOG(
				"Failed to load animation from %s!\n", 
				full_path.c_str()
			);
		}
		return &ref;
	}
	return &it->second;
}