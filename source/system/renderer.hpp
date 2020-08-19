#ifndef LEVIATHAN_INCLUDED_SYSTEM_RENDERER_HPP
#define LEVIATHAN_INCLUDED_SYSTEM_RENDERER_HPP

#include <optional>
#include <glm/mat4x4.hpp>

#include "../utility/enums.hpp"
#include "../resource/pipeline.hpp"
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
	void ortho(glm::ivec2 integral_dimensions);
	arch_t get_draw_calls() const;
	display_list_t& get_overlay_quads(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, const program_t* program, const texture_t* texture, const palette_t* palette);
	display_list_t& get_overlay_quads(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, pipeline_t pipeline, const texture_t* texture, const palette_t* palette);
	display_list_t& get_overlay_quads(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, pipeline_t pipeline);
	display_list_t& get_normal_quads(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, const program_t* program, const texture_t* texture, const palette_t* palette);
	display_list_t& get_normal_quads(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, pipeline_t pipeline, const texture_t* texture, const palette_t* palette);
	display_list_t& get_normal_quads(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, pipeline_t pipeline);
	display_list_t* find_quads(sint64_t guid);
	sint64_t capture_list(display_list_t& list);
	void release_list(display_list_t& list);
private:
	quad_buffer_allocator_t display_allocator;
	std::vector<display_list_t> overlay_quads, normal_quads;
	std::vector<program_t> programs;
	const_buffer_t projection_buffer, viewport_buffer;
	gfx_t graphics_state;
	glm::mat4 gk_projection_matrix, gk_viewport_matrix;
	glm::vec2 gk_video_dimensions, gk_video_resolution;
};

#endif // LEVIATHAN_INCLUDED_SYSTEM_RENDERER_HPP
