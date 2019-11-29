#include "./renderer.hpp"
#include "./video.hpp"

#include "../utl/logger.hpp"
#include "../utl/vfs.hpp"
#include "../gfx/frame_buffer.hpp"

#include <glm/gtc/matrix_transform.hpp>

// static constexpr arch_t kMaxLights = 32;
static constexpr arch_t kMaxIndices = 65535;

renderer_t::renderer_t() :
	//queried_lights(0),
	overlay_quads(),
	normal_quads(),
	//normal_lights(),
	programs(render_pass_t::Total),
	projection_buffer(),
	viewport_buffer(),
	//lighting_buffer(),
	//screen_space(),
	//render_target(),
	graphics_state(),
	gk_projection_matrix(1.0f),
	gk_viewport_matrix(1.0f),
	gk_video_dimensions(320.0f, 180.0f),
	gk_video_resolution(320.0f, 180.0f)
{
	gk_projection_matrix 	= glm::ortho(0.0f, gk_video_resolution.x, gk_video_resolution.y, 0.0f);
	gk_viewport_matrix 		= glm::ortho(0.0f, gk_video_resolution.x, gk_video_resolution.y, 0.0f);
}

renderer_t::~renderer_t() {
	bool result = quad_list_t::release_indexer();
	if (!result) {
		SYNAO_LOG("Couldn't release quad_list_t indexer!\n");
	}
} 

bool renderer_t::init(const setup_file_t&) {
	if (!quad_list_t::allocate_indexer(kMaxIndices, primitive_t::Triangles)) {
		SYNAO_LOG("Couldn't allocate quad_list_t indexer!\n");
		return false;
	}
	/*if (screen_space.valid()) {
		SYNAO_LOG("Screen size vertex buffer already exists!\n");
		return false;
	}
	screen_space.create(4, 4);
	const vtx_minor_t screen_coordinates[4] = {
		vtx_minor_t(-1.0f, 1.0f), 
		vtx_minor_t(-1.0f, -1.0f),
		vtx_minor_t(1.0f, 1.0f), 
		vtx_minor_t(1.0f, -1.0f)
	};
	const uint16 screen_indices[4] = { 0, 1, 2, 3 };
	if (!screen_space.update(screen_coordinates) or !screen_space.update(screen_indices)) {
		SYNAO_LOG("Screen size vertex buffer couldn't update!\n");
		return false;
	}*/
	if (projection_buffer.valid() or viewport_buffer.valid()/* or lighting_buffer.valid()*/) {
		SYNAO_LOG("Constant buffers already exist!\n");
		return false;
	}
	projection_buffer.setup(buffer_usage_t::Static);
	projection_buffer.create(
		sizeof(glm::mat4) + 
		sizeof(glm::vec2) + 
		sizeof(glm::vec2)
	);
	projection_buffer.update(
		&gk_projection_matrix,
		sizeof(glm::mat4)
	);
	projection_buffer.update(
		&gk_video_dimensions,
		sizeof(glm::vec2) + sizeof(glm::vec2),
		sizeof(glm::mat4)
	);
	viewport_buffer.setup(buffer_usage_t::Dynamic);
	viewport_buffer.create(
		sizeof(glm::mat4) + 
		sizeof(glm::vec2) + 
		sizeof(glm::vec2)
	);
	viewport_buffer.update(
		&gk_viewport_matrix,
		sizeof(glm::mat4) + 
		sizeof(glm::vec2) + 
		sizeof(glm::vec2)
	);
	/*lighting_buffer.setup(buffer_usage_t::Dynamic);
	lighting_buffer.create(
		sizeof(glm::uvec4) + 
		sizeof(light_t) * kMaxLights
	);*/
	graphics_state.set_buffer(&viewport_buffer, 0);
	//graphics_state.set_buffer(&lighting_buffer, 1);
	/*if (render_target.valid()) {
		SYNAO_LOG("Frame buffers already exist!\n");
		return false;
	}
	render_target.push(glm::ivec2(gk_video_resolution), 1, pixel_format_t::R8G8B8A0);
	if (!render_target.create()) {
		SYNAO_LOG("Frame buffers couldn't be created!\n");
		return false;
	}*/
	//const shader_t* minor = vfs::shader("minor", shader_stage_t::Vertex);
	const shader_t* blank = vfs::shader("blank", shader_stage_t::Vertex);
	const shader_t* major = vfs::shader("major", shader_stage_t::Vertex);
	bool result = false;
	/*result = programs[render_pass_t::VtxMinorLighting].create(
		minor,
		vfs::shader("lighting", shader_stage_t::Fragment)
	);
	if (!result) {
		SYNAO_LOG("VtxMinorLighting program creation failed!\n");
		return false;
	}*/
	result = programs[render_pass_t::VtxBlankColors].create(
		blank,
		vfs::shader("colors", shader_stage_t::Fragment)
	);
	if (!result) {
		SYNAO_LOG("VtxBlankColors program creation failed!\n");
		return false;
	}
	result = programs[render_pass_t::VtxMajorSprites].create(
		major,
		vfs::shader("sprites", shader_stage_t::Fragment)
	);
	if (!result) {
		SYNAO_LOG("VtxMajorSprites program creation failed!\n");
		return false;
	}
	result = programs[render_pass_t::VtxMajorIndexed].create(
		major,
		vfs::shader("indexed", shader_stage_t::Fragment)
	);
	if (!result) {
		SYNAO_LOG("VtxMajorIndexed program creation failed!\n");
		return false;
	}
	if (!program_t::is_version_420()) {
		programs[render_pass_t::VtxBlankColors].set_block(0, 0);
		programs[render_pass_t::VtxMajorSprites].set_block(0, 0);
		programs[render_pass_t::VtxMajorSprites].set_sampler(0, 0);
		programs[render_pass_t::VtxMajorIndexed].set_block(0, 0);
		programs[render_pass_t::VtxMajorIndexed].set_sampler(0, 0);
		programs[render_pass_t::VtxMajorIndexed].set_sampler(1, 1);
	}
	return true;
}

void renderer_t::clear() {
	//queried_lights = 0;
	overlay_quads.clear();
	normal_quads.clear();
	//normal_lights.clear();
}

void renderer_t::flush(const video_t& video, const glm::mat4& viewport_matrix) {
	// Update Constant Buffers
	glm::vec2 video_dimensions = video.get_dimensions();
	if (gk_video_dimensions != video_dimensions) {
		gk_video_dimensions = video_dimensions;
		projection_buffer.update(&gk_video_dimensions, sizeof(glm::vec2), sizeof(glm::mat4));
		viewport_buffer.update(&gk_video_dimensions, sizeof(glm::vec2), sizeof(glm::mat4));
	}
	if (gk_viewport_matrix != viewport_matrix) {
		gk_viewport_matrix = viewport_matrix;
		viewport_buffer.update(&gk_viewport_matrix, sizeof(glm::mat4));
	}
	// Draw Normal Quads
	// frame_buffer_t::clear(&render_target);
	frame_buffer_t::clear(video.get_integral_dimensions());
	graphics_state.set_buffer(&viewport_buffer, 0);
	for (auto&& batch : normal_quads) {
		batch.flush(&graphics_state);
	}
	// Update Normal Lights
	/*bool_t lighting_buffer_needs_update = false;
	arch_t new_queried_lights = 0;
	for (auto&& light : normal_lights) {
		if (light.query(&new_queried_lights)) {
			lighting_buffer_needs_update = true;
		}
	}
	if (queried_lights != new_queried_lights) {
		queried_lights = new_queried_lights;
		lighting_buffer_needs_update = true;
		uint_t uint_queried_lights = static_cast<uint_t>(queried_lights);
		lighting_buffer.update(&uint_queried_lights, sizeof(uint_t));
	}
	if (lighting_buffer_needs_update) {
		lighting_buffer.update(
			&normal_lights[0],
			sizeof(light_t) * queried_lights,
			sizeof(glm::uvec4)
		);
	}
	// Deferred Rendering / Blitting
	frame_buffer_t::viewport(video.get_integral_dimensions());
	if (queried_lights > 0) {
		frame_buffer_t::bind(nullptr);
		graphics_state.set_blend_mode(blend_mode_t::None);
		graphics_state.set_program(&programs[render_pass_t::VtxMinorLighting]);
		graphics_state.set_sampler(render_target.get_color_buffer(), 0);
		screen_space.draw();
	} else {
		video.submit(&render_target, 0);
	}*/
	// Draw Overlay Quads
	graphics_state.set_buffer(&projection_buffer, 0);
	for (auto&& batch : overlay_quads) {
		batch.flush(&graphics_state);
	}
}

arch_t renderer_t::get_draw_calls() const {
	arch_t result = 0;
	for (auto&& batch : overlay_quads) {
		if (batch.visible()) {
			result++;
		}
	}
	for (auto&& batch : normal_quads) {
		if (batch.visible()) {
			result++;
		}
	}
	return result /*+ queried_lights*/;
}

quad_batch_t& renderer_t::get_overlay_quads(layer_t layer, blend_mode_t blend_mode, const program_t* program, const texture_t* texture, const palette_t* palette) {
	for (auto&& batch : overlay_quads) {
		if (batch.matches(layer, blend_mode, texture, palette, program)) {
			return batch;
		}
	}
	overlay_quads.emplace_back(
		layer, blend_mode, 
		texture, palette, 
		program
	);
	std::sort(overlay_quads.begin(), overlay_quads.end());
	return this->get_overlay_quads(
		layer, blend_mode,
		program, texture, 
		palette
	);
}

quad_batch_t& renderer_t::get_overlay_quads(layer_t layer, blend_mode_t blend_mode, render_pass_t render_pass, const texture_t* texture, const palette_t* palette) {
	return this->get_overlay_quads(
		layer, blend_mode,
		&programs[render_pass], 
		texture, palette
	);
}

quad_batch_t& renderer_t::get_overlay_quads(layer_t layer, blend_mode_t blend_mode, render_pass_t render_pass) {
	return this->get_overlay_quads(
		layer, blend_mode,
		render_pass,
		nullptr, nullptr	
	);
}

quad_batch_t& renderer_t::get_normal_quads(layer_t layer, blend_mode_t blend_mode, const program_t* program, const texture_t* texture, const palette_t* palette) {
	for (auto&& batch : normal_quads) {
		if (batch.matches(layer, blend_mode, texture, palette, program)) {
			return batch;
		}
	}
	normal_quads.emplace_back(
		layer, blend_mode, 
		texture, palette, 
		program
	);
	std::sort(normal_quads.begin(), normal_quads.end());
	return this->get_normal_quads(
		layer, blend_mode,
		program, texture, 
		palette
	);
}

quad_batch_t& renderer_t::get_normal_quads(layer_t layer, blend_mode_t blend_mode, render_pass_t render_pass, const texture_t* texture, const palette_t* palette) {
	return this->get_normal_quads(
		layer, blend_mode,
		&programs[render_pass], 
		texture, palette
	);
}

quad_batch_t& renderer_t::get_normal_quads(layer_t layer, blend_mode_t blend_mode, render_pass_t render_pass) {
	return this->get_normal_quads(
		layer, blend_mode,
		render_pass,
		nullptr, nullptr	
	);
}

/*light_t& renderer_t::get_normal_lights(arch_t index) {
	index = glm::min(index, kMaxLights - 1);
	if (index < normal_lights.size()) {
		return normal_lights[index];
	}
	return normal_lights.emplace_back();
}*/