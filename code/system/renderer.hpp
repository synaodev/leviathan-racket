#ifndef SYNAO_SYSTEM_RENDERER_HPP
#define SYNAO_SYSTEM_RENDERER_HPP

#include <glm/mat4x4.hpp>

#include "../utility/enums.hpp"
#include "../resource/pipeline.hpp"
#include "../video/const_buffer.hpp"
#include "../video/quad_batch.hpp"

struct setup_file_t;
struct video_t;

struct renderer_t : public not_copyable_t {
public:
	renderer_t();
	renderer_t(renderer_t&&) = default;
	renderer_t& operator=(renderer_t&&) = default;
	~renderer_t();
public:
	bool init(glm::ivec2 version);
	void clear();
	void flush(const video_t& video, const glm::mat4& viewport_matrix);
	void flush(const glm::ivec2& dimensions);
	void ortho(glm::ivec2 integral_dimensions);
	arch_t get_draw_calls() const;
	quad_batch_t& get_overlay_quads(layer_t layer, blend_mode_t blend_mode, const program_t* program, const texture_t* texture, const palette_t* palette);
	quad_batch_t& get_overlay_quads(layer_t layer, blend_mode_t blend_mode, pipeline_t pipeline, const texture_t* texture, const palette_t* palette);
	quad_batch_t& get_overlay_quads(layer_t layer, blend_mode_t blend_mode, pipeline_t pipeline);
	quad_batch_t& get_normal_quads(layer_t layer, blend_mode_t blend_mode, const program_t* program, const texture_t* texture, const palette_t* palette);
	quad_batch_t& get_normal_quads(layer_t layer, blend_mode_t blend_mode, pipeline_t pipeline, const texture_t* texture, const palette_t* palette);
	quad_batch_t& get_normal_quads(layer_t layer, blend_mode_t blend_mode, pipeline_t pipeline);
private:
	std::vector<quad_batch_t> overlay_quads, normal_quads;
	std::vector<program_t> programs;
	const_buffer_t projection_buffer, viewport_buffer;
	gfx_t graphics_state;
	glm::mat4 gk_projection_matrix, gk_viewport_matrix;
	glm::vec2 gk_video_dimensions, gk_video_resolution;
};

#endif // SYNAO_SYSTEM_RENDERER_HPP