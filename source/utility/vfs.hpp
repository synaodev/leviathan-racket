#ifndef LEVIATHAN_INCLUDED_UTILITY_VFS_HPP
#define LEVIATHAN_INCLUDED_UTILITY_VFS_HPP

#include <vector>
#include <string>
#include <unordered_map>

#include "./thread_pool.hpp"

#include "../resource/table_entry.hpp"
#include "../video/texture.hpp"
#include "../video/pipeline.hpp"

#if defined(LEVIATHAN_EXECUTABLE_NAOMI)
	#include "../audio/noise.hpp"
	#include "../resource/animation.hpp"
	#include "../system/receiver.hpp"
	#include "../video/font.hpp"
#endif

struct table_entry_t;
struct setup_file_t;
struct vfs_t;

namespace __enum_vfs_resource_path {
	enum type : arch_t {
		Event,
		Field,
		Font,
		I18N,
		Image,
		Init,
		Noise,
		Palette,
		Save,
		Sprite,
		TileKey,
		Tune
	};
}

using vfs_resource_path_t = __enum_vfs_resource_path::type;

namespace vfs {
	static vfs_t* device = nullptr;
	bool mount(const std::string& directory, bool_t print = true);
	bool directory_exists(const std::string& name, bool_t print = true);
	bool file_exists(const std::string& name, bool_t print = true);
	bool create_directory(const std::string& name);
	bool create_recording(const std::string& path, const std::vector<uint16_t>& buffer, sint64_t seed);
	std::string working_directory();
	std::string executable_directory();
	std::string personal_directory();
	std::string resource_path(vfs_resource_path_t path);
	std::vector<std::string> file_list(const std::string& directory);
	std::string string_buffer(const std::string& path);
	std::vector<byte_t> byte_buffer(const std::string& path);
	std::vector<uint_t> uint32_buffer(const std::string& path);
	bool record_buffer(const std::string& path, std::vector<uint16_t>& buffer, sint64_t& seed);
	std::string i18n_find(const std::string& segment, arch_t index);
	std::string i18n_find(const std::string& segment, arch_t first, arch_t last);
	arch_t i18n_size(const std::string& segment);
	bool try_language(const std::string& language);
	const texture_t* texture(const std::vector<std::string>& names, const std::string& directory);
	const texture_t* texture(const std::vector<std::string>& names);
	const texture_t* texture(const std::string& name);
	const palette_t* palette(const std::string& name, const std::string& directory);
	const palette_t* palette(const std::string& name);
	const shader_t* shader(const std::string& name, const std::string& source, shader_stage_t stage);
#if defined(LEVIATHAN_EXECUTABLE_NAOMI)
	std::string event_path(const std::string& name, rec_loading_t flags);
	const noise_t* noise(const std::string& name);
	const noise_t* noise(const table_entry_t& entry);
	const animation_t* animation(const std::string& name);
	const animation_t* animation(const table_entry_t& entry);
	const font_t* font(const std::string& name);
	const font_t* font(arch_t index);
	const font_t* debug_font();
#endif
}

struct vfs_t : public not_copyable_t {
public:
	vfs_t();
	vfs_t(vfs_t&&) = delete;
	vfs_t& operator=(vfs_t&&) = delete;
	~vfs_t();
	bool init(const setup_file_t& config);
	template<typename K, typename T>
	T& emplace_safely(const K& key, std::unordered_map<K, T>& map) {
		std::lock_guard<std::mutex> lock{this->storage_mutex};
		auto result = map.try_emplace(key);
		return result.first->second;
	}
	template<typename K, typename T>
	auto search_safely(const K& key, const std::unordered_map<K, T>& map) {
		std::lock_guard<std::mutex> lock{this->storage_mutex};
		return map.find(key);
	}
public:
	std::unique_ptr<thread_pool_t> thread_pool;
	std::mutex storage_mutex;
	std::string personal, language;
	std::unordered_map<std::string, std::vector<std::string> > i18n;
	std::unordered_map<std::string, texture_t> textures;
	std::unordered_map<std::string, palette_t> palettes;
	std::unordered_map<std::string, shader_t> shaders;
#if defined(LEVIATHAN_EXECUTABLE_NAOMI)
	std::unordered_map<arch_t, noise_t> noises;
	std::unordered_map<arch_t, animation_t> animations;
	std::unordered_map<std::string, font_t> fonts;
#endif
};

#endif // LEVIATHAN_INCLUDED_UTILITY_VFS_HPP
