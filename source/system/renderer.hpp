#ifndef LEVIATHAN_INCLUDED_SYSTEM_RENDERER_HPP
#define LEVIATHAN_INCLUDED_SYSTEM_RENDERER_HPP

#include <glm/mat4x4.hpp>

#include "../resource/pipeline.hpp"
#include "../utility/enums.hpp"
#include "../video/const_buffer.hpp"
#include "../video/display_list.hpp"

struct setup_file_t;
struct video_t;

struct renderer_t : public not_copyable_t {
public:
	renderer_t();
	renderer_t(renderer_t&&) = default;
	renderer_t& operator=(renderer_t&&) = default;
	~renderer_t() = default;
public:
	bool init(glm::ivec2 version);
	void clear();
	void flush(const video_t& video, const glm::mat4& viewport_matrix);
	void flush(const glm::ivec2& dimensions);
	void ortho(const glm::ivec2& integral_dimensions);
	arch_t get_total_lists() const;
	arch_t get_total_calls() const;
	display_list_t* find_list(sint64_t guid);
	sint64_t capture(display_list_t& list);
	void release(display_list_t& list);
	display_list_t& overlay_list(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, const program_t* program, const texture_t* texture, const palette_t* palette);
	display_list_t& overlay_list(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, pipeline_t pipeline, const texture_t* texture, const palette_t* palette);
	display_list_t& overlay_list(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, pipeline_t pipeline);
	display_list_t& working_list(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, const program_t* program, const texture_t* texture, const palette_t* palette);
	display_list_t& working_list(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, pipeline_t pipeline, const texture_t* texture, const palette_t* palette);
	display_list_t& working_list(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, pipeline_t pipeline);
private:
	quad_allocator_t quad_allocator;
	std::vector<display_list_t> overlay_lists, working_lists;
	std::vector<program_t> programs;
	const_buffer_t projection_buffer, viewport_buffer;
	gfx_t internal_state;
	glm::mat4 gk_projection_matrix, gk_viewport_matrix;
	glm::vec2 gk_video_dimensions, gk_video_resolution;
};

#endif // LEVIATHAN_INCLUDED_SYSTEM_RENDERER_HPP
