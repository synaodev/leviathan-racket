#ifndef SYNAO_UTILITY_VFS_HPP
#define SYNAO_UTILITY_VFS_HPP

#include <vector>
#include <string>
#include <unordered_map>

#include "../audio/noise.hpp"
#include "../video/texture.hpp"
#include "../video/palette.hpp"
#include "../video/program.hpp"
#include "../video/font.hpp"
#include "../video/animation.hpp"
#include "../event/receiver.hpp"
#include "./thread_pool.hpp"

struct resource_entry_t;
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
	std::back_insert_iterator<std::u32string> to_utf32(
		std::string::const_iterator begin, 
		std::string::const_iterator end, 
		std::back_insert_iterator<std::u32string> output
	);
	bool verify_structure();
	bool directory_exists(const std::string& name);
	bool create_directory(const std::string& name);
	std::string resource_path(vfs_resource_path_t path);
	std::vector<std::string> file_list(const std::string& directory);
	std::string string_buffer(const std::string& path);
	std::vector<byte_t> byte_buffer(const std::string& path);
	std::vector<sint_t> sint_buffer(const std::string& path);
	std::string event_path(const std::string& name, rec_loading_t flags);
	std::string i18n_find(const std::string& segment, arch_t index);
	std::string i18n_find(const std::string& segment, arch_t first, arch_t last);
	arch_t i18n_size(const std::string& segment);
	bool try_language(const std::string& language);
	std::string local_script(const std::string& name);
	std::string global_script(const std::string& name);
	const noise_t* noise(const std::string& name);
	const noise_t* noise(const resource_entry_t& entry);
	const texture_t* texture(const std::vector<std::string>& names, const std::string& directory);
	const texture_t* texture(const std::vector<std::string>& names);
	const texture_t* texture(const std::string& name);
	const palette_t* palette(const std::string& name, const std::string& directory);
	const palette_t* palette(const std::string& name);
	const shader_t* shader(const std::string& name, const std::string& source, shader_stage_t stage);
	const font_t* font(const std::string& name);
	const font_t* font(arch_t index);
	const animation_t* animation(const std::string& name);
}

struct vfs_t : public not_copyable_t {
public:
	vfs_t();
	vfs_t(vfs_t&&) = delete;
	vfs_t& operator=(vfs_t&&) = delete;
	~vfs_t();
	bool mount(const setup_file_t& config);
	template<typename K, typename T>
	T& allocate_safely(const K& key, std::unordered_map<K, T>& map) {
		std::lock_guard<std::mutex> lock{this->storage_mutex};
		auto result = map.try_emplace(key);
		return result.first->second;
	}
	// template<typename K, typename T>
	// auto search_safely(const K& key, const std::unordered_map<K, T>& map) {
	// 	std::lock_guard<std::mutex> lock{this->storage_mutex};
	// 	return map.find(key);
	// }
public:
	std::unique_ptr<thread_pool_t> thread_pool;
	std::mutex storage_mutex;
	std::string language;
	std::unordered_map<std::string, std::vector<std::string> > i18n;
	std::unordered_map<arch_t, noise_t> noises;
	std::unordered_map<std::string, texture_t> textures;
	std::unordered_map<std::string, palette_t> palettes;
	std::unordered_map<std::string, shader_t> shaders;
	std::unordered_map<std::string, font_t> fonts;
	std::unordered_map<std::string, animation_t> animations;
};

#endif // SYNAO_UTILITY_VFS_HPP