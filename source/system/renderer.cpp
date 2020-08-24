#include "./renderer.hpp"
#include "./video.hpp"

#include "../utility/constants.hpp"
#include "../utility/logger.hpp"
#include "../utility/vfs.hpp"
#include "../video/frame_buffer.hpp"

#include <limits>
#include <glm/gtc/matrix_transform.hpp>

renderer_t::renderer_t() :
	display_allocator(),
	overlay_quads(),
	normal_quads(),
	programs(pipeline_t::Total),
	projection_buffer(),
	viewport_buffer(),
	graphics_state(),
	gk_projection_matrix(1.0f),
	gk_viewport_matrix(1.0f),
	gk_video_dimensions(constants::NormalDimensions<real_t>()),
	gk_video_resolution(constants::NormalDimensions<real_t>())
{
	gk_projection_matrix = glm::ortho(0.0f, gk_video_resolution.x, gk_video_resolution.y, 0.0f);
	gk_viewport_matrix = gk_projection_matrix;
}

bool renderer_t::init(glm::ivec2 version) {
	if (!display_allocator.create(primitive_t::Triangles, UINT16_MAX)) {
		synao_log("Couldn't create quad_buffer_allocator_t!\n");
		return false;
	}
	if (projection_buffer.valid() or viewport_buffer.valid()) {
		synao_log("Constant buffers already exist!\n");
		return false;
	}
	projection_buffer.setup(buffer_usage_t::Static);
	if (const_buffer_t::has_immutable_option()) {
		projection_buffer.create_immutable(
			sizeof(glm::mat4) +
			sizeof(glm::vec2) +
			sizeof(glm::vec2)
		);
	} else {
		projection_buffer.create(
			sizeof(glm::mat4) +
			sizeof(glm::vec2) +
			sizeof(glm::vec2)
		);
	}
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
	if (const_buffer_t::has_immutable_option()) {
		viewport_buffer.create_immutable(
			sizeof(glm::mat4) +
			sizeof(glm::vec2) +
			sizeof(glm::vec2)
		);
	} else {
		viewport_buffer.create(
			sizeof(glm::mat4) +
			sizeof(glm::vec2) +
			sizeof(glm::vec2)
		);
	}
	viewport_buffer.update(
		&gk_viewport_matrix,
		sizeof(glm::mat4) +
		sizeof(glm::vec2) +
		sizeof(glm::vec2)
	);
	graphics_state.set_const_buffer(&viewport_buffer, 0);

	const shader_t* blank = vfs::shader(
		"blank",
		pipeline::blank_vert(version),
		shader_stage_t::Vertex
	);
	const shader_t* major = vfs::shader(
		"major",
		pipeline::major_vert(version),
		shader_stage_t::Vertex
	);
	const shader_t* colors = vfs::shader(
		"colors",
		pipeline::colors_frag(version),
		shader_stage_t::Fragment
	);
	const shader_t* sprites = vfs::shader(
		"sprites",
		pipeline::sprites_frag(version),
		shader_stage_t::Fragment
	);
	const shader_t* indexed = vfs::shader(
		"indexed",
		pipeline::indexed_frag(version),
		shader_stage_t::Fragment
	);
	bool result = programs[pipeline_t::VtxBlankColors].create(blank, colors);
	if (!result) {
		synao_log("VtxBlankColors program creation failed!\n");
		return false;
	}
	result = programs[pipeline_t::VtxMajorSprites].create(major, sprites);
	if (!result) {
		synao_log("VtxMajorSprites program creation failed!\n");
		return false;
	}
	result = programs[pipeline_t::VtxMajorIndexed].create(major, indexed);
	if (!result) {
		synao_log("VtxMajorIndexed program creation failed!\n");
		return false;
	}
	if (!program_t::has_separable()) {
		programs[pipeline_t::VtxBlankColors].set_block("transforms", 0);
		programs[pipeline_t::VtxMajorSprites].set_block("transforms", 0);
		programs[pipeline_t::VtxMajorSprites].set_sampler("diffuse_map", 0);
		programs[pipeline_t::VtxMajorIndexed].set_block("transforms", 0);
		programs[pipeline_t::VtxMajorIndexed].set_sampler("indexed_map", 0);
		programs[pipeline_t::VtxMajorIndexed].set_sampler("palette_map", 1);
	}
	synao_log("Rendering service is ready.\n");
	return true;
}

void renderer_t::clear() {
	auto lacks_owner = [](auto& list) { return !list.persists(); };
	overlay_quads.erase(
		std::remove_if(overlay_quads.begin(), overlay_quads.end(), lacks_owner),
		overlay_quads.end()
	);
	normal_quads.erase(
		std::remove_if(normal_quads.begin(), normal_quads.end(), lacks_owner),
		normal_quads.end()
	);
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
	frame_buffer_t::clear(video.get_integral_dimensions());
	graphics_state.set_const_buffer(&viewport_buffer, 0);
	for (auto&& list : normal_quads) {
		list.flush(graphics_state);
	}
	// Draw Overlay Quads
	graphics_state.set_const_buffer(&projection_buffer, 0);
	for (auto&& list : overlay_quads) {
		list.flush(graphics_state);
	}
}

void renderer_t::flush(const glm::ivec2& dimensions) {
	// Draw Overlay Quads (Only)
	frame_buffer_t::clear(dimensions);
	graphics_state.set_const_buffer(&projection_buffer, 0);
	for (auto&& list : overlay_quads) {
		list.flush(graphics_state);
	}
}

void renderer_t::ortho(glm::ivec2 integral_dimensions) {
	if (projection_buffer.valid() and viewport_buffer.valid()) {
		glm::vec2 dimensions = glm::vec2(integral_dimensions);
		gk_projection_matrix = glm::ortho(0.0f, dimensions.x, dimensions.y, 0.0f);
		gk_viewport_matrix = gk_projection_matrix;
		gk_video_dimensions = dimensions;
		gk_video_resolution = dimensions;
		projection_buffer.update(
			&gk_projection_matrix,
			sizeof(glm::mat4)
		);
		projection_buffer.update(
			&gk_video_dimensions,
			sizeof(glm::vec2) + sizeof(glm::vec2),
			sizeof(glm::mat4)
		);
		viewport_buffer.update(
			&gk_viewport_matrix,
			sizeof(glm::mat4) +
			sizeof(glm::vec2) +
			sizeof(glm::vec2)
		);
	}
}

arch_t renderer_t::get_draw_calls() const {
	arch_t result = 0;
	for (auto&& list : overlay_quads) {
		if (list.rendered()) {
			result++;
		}
	}
	for (auto&& list : normal_quads) {
		if (list.rendered()) {
			result++;
		}
	}
	return result;
}

display_list_t& renderer_t::get_overlay_quads(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, const program_t* program, const texture_t* texture, const palette_t* palette) {
	for (auto&& list : overlay_quads) {
		if (list.matches(layer, blend_mode, usage, texture, palette, program)) {
			return list;
		}
	}
	overlay_quads.emplace_back(
		layer, blend_mode, usage,
		texture, palette,
		program, &display_allocator
	);
	std::sort(overlay_quads.begin(), overlay_quads.end());
	return this->get_overlay_quads(
		layer, blend_mode, usage,
		program, texture,
		palette
	);
}

display_list_t& renderer_t::get_overlay_quads(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, pipeline_t pipeline, const texture_t* texture, const palette_t* palette) {
	return this->get_overlay_quads(
		layer, blend_mode, usage,
		&programs[pipeline],
		texture, palette
	);
}

display_list_t& renderer_t::get_overlay_quads(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, pipeline_t pipeline) {
	return this->get_overlay_quads(
		layer, blend_mode, usage,
		pipeline,
		nullptr, nullptr
	);
}

display_list_t& renderer_t::get_normal_quads(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, const program_t* program, const texture_t* texture, const palette_t* palette) {
	for (auto&& list : normal_quads) {
		if (list.matches(layer, blend_mode, usage, texture, palette, program)) {
			return list;
		}
	}
	normal_quads.emplace_back(
		layer, blend_mode, usage,
		texture, palette,
		program, &display_allocator
	);
	std::sort(normal_quads.begin(), normal_quads.end());
	return this->get_normal_quads(
		layer, blend_mode, usage,
		program, texture,
		palette
	);
}

display_list_t& renderer_t::get_normal_quads(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, pipeline_t pipeline, const texture_t* texture, const palette_t* palette) {
	return this->get_normal_quads(
		layer, blend_mode, usage,
		&programs[pipeline],
		texture, palette
	);
}

display_list_t& renderer_t::get_normal_quads(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, pipeline_t pipeline) {
	return this->get_normal_quads(
		layer, blend_mode, usage,
		pipeline,
		nullptr, nullptr
	);
}

display_list_t* renderer_t::find_quads(sint64_t guid) {
	if (guid != 0) {
		for (auto&& list : normal_quads) {
			if (list.matches(guid)) {
				return &list;
			}
		}
		for (auto&& list : overlay_quads) {
			if (list.matches(guid)) {
				return &list;
			}
		}
	}
	return nullptr;
}

sint64_t renderer_t::capture_list(display_list_t& list) {
	sint64_t guid = list.capture(graphics_state);
#ifdef LEVIATHAN_BUILD_DEBUG
	assert(guid != 0);
#endif
	return guid;
}

void renderer_t::release_list(display_list_t& list) {
	bool success = list.release(graphics_state);
#ifdef LEVIATHAN_BUILD_DEBUG
	assert(success);
#endif
}
