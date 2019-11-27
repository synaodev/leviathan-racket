#ifndef SYNAO_MAIN_RENDERER_HPP
#define SYNAO_MAIN_RENDERER_HPP

#include <glm/mat4x4.hpp>

#include "../utl/enums.hpp"
#include "../gfx/const_buffer.hpp"
//#include "../gfx/frame_buffer.hpp"
#include "../gfx/quad_batch.hpp"
//#include "../gfx/light.hpp"

namespace __enum_render_pass {
	enum type : arch_t {
		//VtxMinorLighting,
		VtxBlankColors,
		VtxMajorSprites,
		VtxMajorIndexed,
		Total
	};
}

using render_pass_t = __enum_render_pass::type;

struct setup_file_t;
struct video_t;

struct renderer_t : public not_copyable_t, public not_moveable_t {
public:
	renderer_t();
	~renderer_t();
public:
	bool init(const setup_file_t& config);
	void clear();
	void flush(const video_t& video, const glm::mat4& viewport_matrix);
	arch_t get_draw_calls() const;
	quad_batch_t& get_overlay_quads(layer_t layer, blend_mode_t blend_mode, const program_t* program, const texture_t* texture, const palette_t* palette);
	quad_batch_t& get_overlay_quads(layer_t layer, blend_mode_t blend_mode, render_pass_t render_pass, const texture_t* texture, const palette_t* palette);
	quad_batch_t& get_overlay_quads(layer_t layer, blend_mode_t blend_mode, render_pass_t render_pass);
	quad_batch_t& get_normal_quads(layer_t layer, blend_mode_t blend_mode, const program_t* program, const texture_t* texture, const palette_t* palette);
	quad_batch_t& get_normal_quads(layer_t layer, blend_mode_t blend_mode, render_pass_t render_pass, const texture_t* texture, const palette_t* palette);
	quad_batch_t& get_normal_quads(layer_t layer, blend_mode_t blend_mode, render_pass_t render_pass);
	//light_t& get_normal_lights(arch_t index);
private:
	//arch_t queried_lights;
	std::vector<quad_batch_t> overlay_quads, normal_quads;
	//std::vector<light_t> normal_lights;
	std::vector<program_t> programs;
	const_buffer_t projection_buffer, viewport_buffer/*, lighting_buffer*/;
	//vertex_buffer_t screen_space;
	//frame_buffer_t render_target;
	gfx_t graphics_state;
private: // For Constant Buffers
	glm::mat4 gk_projection_matrix, gk_viewport_matrix;
	glm::vec2 gk_video_dimensions, gk_video_resolution;
};

#endif // SYNAO_MAIN_RENDER_HPP