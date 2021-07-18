#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <entt/core/hashed_string.hpp>

#include "./animation.hpp"
#include "./font.hpp"

#include "../audio/noise.hpp"
#include "../utility/thread-pool.hpp"
#include "../video/texture.hpp"
#include "../video/pipeline.hpp"

struct config_t;
struct vfs_t;

enum class vfs_resource_path_t : arch_t {
	Event, Field, Font,
	I18N, Image, Init,
	Noise, Save, Sprite,
	TileKey, Tune
};

struct vfs_t : public not_copyable_t, public not_moveable_t {
public:
	vfs_t() = default;
	~vfs_t();
public:
	bool init(const config_t& config);
	bool set_sampler_allocator(sampler_allocator_t* sampler_allocator);
public:
	static bool mount(const std::string& directory, bool_t print = true);
	static bool directory_exists(const std::string& name, bool_t print = true);
	static bool file_exists(const std::string& name, bool_t print = true);
	static bool create_directory(const std::string& name);
	static bool create_recording(const std::string& path, const std::vector<uint16_t>& buffer, sint64_t seed);
	static std::string working_directory();
	static std::string executable_directory();
	static std::string personal_directory();
	static std::string resource_path(vfs_resource_path_t path);
	static std::vector<std::string> file_list(const std::string& directory);
	static std::string string_buffer(const std::string& path);
	static std::vector<byte_t> byte_buffer(const std::string& path);
	static std::vector<uint_t> uint32_buffer(const std::string& path);
	static bool record_buffer(const std::string& path, std::vector<uint16_t>& buffer, sint64_t& seed);
	static std::string i18n_find(const std::string& segment, arch_t index);
	static std::string i18n_find(const std::string& segment, arch_t first, arch_t last);
	static arch_t i18n_size(const std::string& segment);
	static bool try_language(const std::string& language);
	static const texture_t* texture(const std::string& name);
	static const atlas_t* atlas(const std::string& name);
	static const shader_t* shader(const std::string& name, const std::string& source, shader_stage_t stage);
	static std::string event_path(const std::string& name, event_loading_t flags);
	static const noise_t* noise(const std::string& name);
	static const noise_t* noise(const entt::hashed_string& entry);
	static const animation_t* animation(const std::string& name);
	static const animation_t* animation(const entt::hashed_string& entry);
	static const font_t* font(const std::string& name);
	static const font_t* font(arch_t index);
	static const font_t* debug_font();
private:
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
private:
	static vfs_t* device;
	thread_pool_t thread_pool {};
	std::mutex storage_mutex {};
	std::string personal {};
	std::string language {};
	sampler_allocator_t* sampler_allocator { nullptr };
	std::unordered_map<std::string, std::vector<std::string> > i18n {};
	std::unordered_map<std::string, texture_t> textures {};
	std::unordered_map<std::string, atlas_t> atlases {};
	std::unordered_map<std::string, shader_t> shaders {};
	std::unordered_map<entt::id_type, noise_t> noises {};
	std::unordered_map<entt::id_type, animation_t> animations {};
	std::unordered_map<std::string, font_t> fonts {};
};
