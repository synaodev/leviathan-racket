#include "./renderer.hpp"
#include "./video.hpp"

#include "../utility/constants.hpp"
#include "../utility/logger.hpp"
#include "../utility/vfs.hpp"
#include "../video/frame-buffer.hpp"

#include <limits>
#include <glm/gtc/matrix_transform.hpp>

renderer_t::renderer_t() :
	quad_allocator(),
	overlay_lists(),
	working_lists(),
	pipelines(program_t::Total),
	projection_buffer(),
	viewport_buffer(),
	internal_state(),
	gk_projection_matrix(1.0f),
	gk_viewport_matrix(1.0f)
{
	const glm::vec2 resolution = constants::NormalDimensions<real_t>();
	gk_projection_matrix = glm::ortho(0.0f, resolution.x, resolution.y, 0.0f);
	gk_viewport_matrix = gk_projection_matrix;
}

bool renderer_t::init(glm::ivec2 version) {
	if (!quad_allocator.create(primitive_t::Triangles, UINT16_MAX)) {
		synao_log("Couldn't create quad_buffer_allocator_t!\n");
		return false;
	}
	if (projection_buffer.valid() or viewport_buffer.valid()) {
		synao_log("Constant buffers already exist!\n");
		return false;
	}
	projection_buffer.setup(buffer_usage_t::Static);
	if (const_buffer_t::has_immutable_option()) {
		projection_buffer.create_immutable(sizeof(glm::mat4));
	} else {
		projection_buffer.create(sizeof(glm::mat4));
	}
	projection_buffer.update(&gk_projection_matrix);
	viewport_buffer.setup(buffer_usage_t::Dynamic);
	if (const_buffer_t::has_immutable_option()) {
		viewport_buffer.create_immutable(sizeof(glm::mat4));
	} else {
		viewport_buffer.create(sizeof(glm::mat4));
	}
	viewport_buffer.update(&gk_viewport_matrix);
	internal_state.set_const_buffer(&viewport_buffer, 0);

	const shader_t* blank = vfs::shader(
		"blank",
		program::blank_vert(version),
		shader_stage_t::Vertex
	);
	const shader_t* major = vfs::shader(
		"major",
		program::major_vert(version),
		shader_stage_t::Vertex
	);
	const shader_t* colors = vfs::shader(
		"colors",
		program::colors_frag(version),
		shader_stage_t::Fragment
	);
	const shader_t* sprites = vfs::shader(
		"sprites",
		program::sprites_frag(version),
		shader_stage_t::Fragment
	);
	const shader_t* indexed = vfs::shader(
		"indexed",
		program::indexed_frag(version),
		shader_stage_t::Fragment
	);
	bool result = pipelines[program_t::Colors].create(blank, colors);
	if (!result) {
		synao_log("\"Colors\" program creation failed!\n");
		return false;
	}
	result = pipelines[program_t::Sprites].create(major, sprites);
	if (!result) {
		synao_log("\"Sprites\" program creation failed!\n");
		return false;
	}
	result = pipelines[program_t::Indexed].create(major, indexed);
	if (!result) {
		synao_log("\"Indexed\" program creation failed!\n");
		return false;
	}
	if (!pipeline_t::has_separable()) {
		pipelines[program_t::Colors].set_block("transforms", 0);
		pipelines[program_t::Sprites].set_block("transforms", 0);
		pipelines[program_t::Sprites].set_sampler("diffuse_map", 0);
		pipelines[program_t::Indexed].set_block("transforms", 0);
		pipelines[program_t::Indexed].set_sampler("indexed_map", 0);
		pipelines[program_t::Indexed].set_sampler("palette_map", 1);
	}
	synao_log("Rendering service is ready.\n");
	return true;
}

void renderer_t::clear() {
	auto lacks_owner = [](auto& list) { return !list.persists(); };
	overlay_lists.erase(
		std::remove_if(overlay_lists.begin(), overlay_lists.end(), lacks_owner),
		overlay_lists.end()
	);
	working_lists.erase(
		std::remove_if(working_lists.begin(), working_lists.end(), lacks_owner),
		working_lists.end()
	);
}

void renderer_t::flush(const video_t& video, const glm::mat4& viewport_matrix) {
	// Update Constant Buffers
	// glm::vec2 video_dimensions = video.get_dimensions();
	// if (gk_video_dimensions != video_dimensions) {
	// 	gk_video_dimensions = video_dimensions;
	// 	projection_buffer.update(&gk_video_dimensions, sizeof(glm::vec2), sizeof(glm::mat4));
	// 	viewport_buffer.update(&gk_video_dimensions, sizeof(glm::vec2), sizeof(glm::mat4));
	// }
	if (gk_viewport_matrix != viewport_matrix) {
		gk_viewport_matrix = viewport_matrix;
		viewport_buffer.update(&gk_viewport_matrix, sizeof(glm::mat4));
	}
	// Draw Normal Quads
	frame_buffer_t::clear(video.get_integral_dimensions());
	internal_state.set_const_buffer(&viewport_buffer, 0);
	for (auto&& list : working_lists) {
		list.flush(internal_state);
	}
	// Draw Overlay Quads
	internal_state.set_const_buffer(&projection_buffer, 0);
	for (auto&& list : overlay_lists) {
		list.flush(internal_state);
	}
}

void renderer_t::flush(const glm::ivec2& dimensions) {
	// Draw Overlay Quads (Only)
	frame_buffer_t::clear(dimensions);
	internal_state.set_const_buffer(&projection_buffer, 0);
	for (auto&& list : overlay_lists) {
		list.flush(internal_state);
	}
}

void renderer_t::ortho(const glm::ivec2& integral_dimensions) {
	if (projection_buffer.valid() and viewport_buffer.valid()) {
		glm::vec2 dimensions = glm::vec2(integral_dimensions);
		gk_projection_matrix = glm::ortho(0.0f, dimensions.x, dimensions.y, 0.0f);
		gk_viewport_matrix = gk_projection_matrix;
		//gk_video_dimensions = dimensions;
		//gk_video_resolution = dimensions;
		projection_buffer.update(&gk_projection_matrix, sizeof(glm::mat4));
		// projection_buffer.update(
		// 	&gk_video_dimensions,
		// 	sizeof(glm::vec2) + sizeof(glm::vec2),
		// 	sizeof(glm::mat4)
		// );
		viewport_buffer.update(&gk_viewport_matrix, sizeof(glm::mat4));
	}
}

arch_t renderer_t::get_total_lists() const {
	return overlay_lists.size() + working_lists.size();
}

arch_t renderer_t::get_total_calls() const {
	arch_t result = 0;
	for (auto&& list : overlay_lists) {
		if (list.rendered()) {
			result++;
		}
	}
	for (auto&& list : working_lists) {
		if (list.rendered()) {
			result++;
		}
	}
	return result;
}

display_list_t* renderer_t::find_list(sint64_t guid) {
	if (guid != 0) {
		for (auto&& list : working_lists) {
			if (list.matches(guid)) {
				return &list;
			}
		}
		for (auto&& list : overlay_lists) {
			if (list.matches(guid)) {
				return &list;
			}
		}
	}
	return nullptr;
}

sint64_t renderer_t::capture(display_list_t& list) {
	sint64_t guid = list.capture(internal_state);
#ifdef LEVIATHAN_BUILD_DEBUG
	assert(guid != 0);
#endif
	return guid;
}

void renderer_t::release(display_list_t& list) {
	bool success = list.release(internal_state);
#ifdef LEVIATHAN_BUILD_DEBUG
	assert(success);
#endif
}

display_list_t& renderer_t::overlay_list(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, const pipeline_t* pipeline, const texture_t* texture, const palette_t* palette) {
	for (auto&& list : overlay_lists) {
		if (list.matches(layer, blend_mode, usage, texture, palette, pipeline)) {
			return list;
		}
	}
	overlay_lists.emplace_back(
		layer, blend_mode, usage,
		texture, palette,
		pipeline, &quad_allocator
	);
	std::sort(overlay_lists.begin(), overlay_lists.end());
	return this->overlay_list(
		layer, blend_mode, usage,
		pipeline, texture, palette
	);
}

display_list_t& renderer_t::overlay_list(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, program_t program, const texture_t* texture, const palette_t* palette) {
	return this->overlay_list(
		layer, blend_mode, usage,
		&pipelines[program],
		texture, palette
	);
}

display_list_t& renderer_t::overlay_list(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, program_t program) {
	return this->overlay_list(
		layer, blend_mode, usage,
		program, nullptr, nullptr
	);
}

display_list_t& renderer_t::working_list(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, const pipeline_t* pipeline, const texture_t* texture, const palette_t* palette) {
	for (auto&& list : working_lists) {
		if (list.matches(layer, blend_mode, usage, texture, palette, pipeline)) {
			return list;
		}
	}
	working_lists.emplace_back(
		layer, blend_mode, usage,
		texture, palette,
		pipeline, &quad_allocator
	);
	std::sort(working_lists.begin(), working_lists.end());
	return this->working_list(
		layer, blend_mode, usage,
		pipeline, texture, palette
	);
}

display_list_t& renderer_t::working_list(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, program_t program, const texture_t* texture, const palette_t* palette) {
	return this->working_list(
		layer, blend_mode, usage,
		&pipelines[program],
		texture, palette
	);
}

display_list_t& renderer_t::working_list(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, program_t program) {
	return this->working_list(
		layer, blend_mode, usage,
		program, nullptr, nullptr
	);
}
