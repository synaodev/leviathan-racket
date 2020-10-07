#ifndef LEVIATHAN_INCLUDED_SYSTEM_RENDERER_HPP
#define LEVIATHAN_INCLUDED_SYSTEM_RENDERER_HPP

#include <glm/mat4x4.hpp>

#include "../resource/program.hpp"
#include "../utility/enums.hpp"
#include "../video/const-buffer.hpp"
#include "../video/display-list.hpp"

struct setup_file_t;
struct vfs_t;
struct video_t;

struct renderer_t : public not_copyable_t {
public:
	renderer_t();
	renderer_t(renderer_t&&) = default;
	renderer_t& operator=(renderer_t&&) = default;
	~renderer_t() = default;
public:
	bool init(glm::ivec2 version, vfs_t& fs);
	void clear();
	void flush(const video_t& video, const glm::mat4& viewport);
	void flush(const glm::ivec2& dimensions);
	void ortho(const glm::ivec2& dimensions);
	arch_t get_total_lists() const;
	arch_t get_total_calls() const;
	display_list_t& display_list(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, program_t program);
private:
	quad_allocator_t quad_allocator;
	sampler_allocator_t sampler_allocator;
	std::vector<display_list_t> display_lists;
	std::vector<pipeline_t> pipelines;
	const_buffer_t viewports;
	gfx_t internal_state;
};

#endif // LEVIATHAN_INCLUDED_SYSTEM_RENDERER_HPP
