#include "./renderer.hpp"
#include "./video.hpp"

#include "../resource/vfs.hpp"
#include "../utility/constants.hpp"
#include "../utility/logger.hpp"
#include "../video/frame-buffer.hpp"

#include <limits>
#include <glm/gtc/matrix_transform.hpp>

bool renderer_t::init(vfs_t& fs) {
	arch_t amount = static_cast<arch_t>(std::numeric_limits<uint16_t>::max());
	if (!quad_allocator.create(primitive_t::Triangles, amount)) {
		synao_log("Couldn't setup quad_buffer_allocator_t!\n");
		return false;
	}
	if (!sampler_allocator.create(pixel_format_t::R8G8B8A8)) {
		synao_log("Couldn't setup sampler_allocator_t!\n");
		return false;
	}
	if (!fs.set_sampler_allocator(&sampler_allocator)) {
		synao_log("Couldn't set the virtual filesystem's sampler allocator!\n");
		return false;
	}
	pipelines.resize(program_t::Total);
	if (viewports.valid()) {
		synao_log("Constant buffers already exist!\n");
		return false;
	}
	viewports.setup(buffer_usage_t::Dynamic);
	if (const_buffer_t::has_immutable_option()) {
		viewports.create_immutable(sizeof(glm::mat4) * 2);
	} else {
		viewports.create(sizeof(glm::mat4) * 2);
	}
	glm::mat4 matrices[2] = {
		glm::ortho(0.0f, constants::NormalWidth<real_t>(), constants::NormalHeight<real_t>(), 0.0f),
		glm::ortho(0.0f, constants::NormalWidth<real_t>(), constants::NormalHeight<real_t>(), 0.0f)
	};
	viewports.update(matrices);
	internal_state.set_buffer(&viewports, 0);

	const shader_t* blank = vfs_t::shader(
		"blank",
		program::blank_vert(),
		shader_stage_t::Vertex
	);
	const shader_t* major = vfs_t::shader(
		"major",
		program::major_vert(),
		shader_stage_t::Vertex
	);
	const shader_t* fonts = vfs_t::shader(
		"fonts",
		program::fonts_vert(),
		shader_stage_t::Vertex
	);
	const shader_t* colors = vfs_t::shader(
		"colors",
		program::colors_frag(),
		shader_stage_t::Fragment
	);
	const shader_t* sprites = vfs_t::shader(
		"sprites",
		program::sprites_frag(),
		shader_stage_t::Fragment
	);
	const shader_t* channels = vfs_t::shader(
		"channels",
		program::channels_frag(),
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
	result = pipelines[program_t::Strings].create(fonts, channels);
	if (!result) {
		synao_log("\"Strings\" program creation failed!\n");
	}
	if (!pipeline_t::has_separable()) {
		pipelines[program_t::Colors].set_block("transforms", 0);
		pipelines[program_t::Sprites].set_block("transforms", 0);
		pipelines[program_t::Sprites].set_sampler("diffuse", 0);
		pipelines[program_t::Strings].set_block("transforms", 0);
		pipelines[program_t::Strings].set_sampler("channels", 1);
	}
	synao_log("Rendering service is ready.\n");
	return true;
}

void renderer_t::clear() {
	display_lists.clear();
}

void renderer_t::flush(const video_t& video, const glm::mat4& viewport) {
	// Update Viewports
	static glm::mat4 current { 1.0f };
	if (current != viewport) {
		current = viewport;
		viewports.update(&current, sizeof(glm::mat4), sizeof(glm::mat4));
	}
	this->flush(video.get_integral_dimensions());
}

void renderer_t::flush(const glm::ivec2& dimensions) {
	// Draw Lists
	frame_buffer::clear(dimensions);
	for (auto&& list : display_lists) {
		list.flush(internal_state, &sampler_allocator);
	}
}

void renderer_t::ortho(const glm::ivec2& dimensions) {
	if (viewports.valid()) {
		glm::mat4 matrices[] = {
			glm::ortho(0.0f, static_cast<real_t>(dimensions.x), static_cast<real_t>(dimensions.y), 0.0f),
			glm::ortho(0.0f, static_cast<real_t>(dimensions.x), static_cast<real_t>(dimensions.y), 0.0f)
		};
		viewports.update(matrices);
	}
}

arch_t renderer_t::get_total_lists() const {
	return display_lists.size();
}

arch_t renderer_t::get_total_calls() const {
	arch_t result = 0;
	for (auto&& list : display_lists) {
		if (list.rendered()) {
			result++;
		}
	}
	return result;
}

display_list_t& renderer_t::display_list(layer_t layer, blend_mode_t blend_mode, program_t program) {
	for (auto&& list : display_lists) {
		if (list.matches(layer, blend_mode, &pipelines[program])) {
			return list;
		}
	}
	display_lists.emplace_back(
		layer, blend_mode,
		&pipelines[program]
	);
	std::sort(display_lists.begin(), display_lists.end());
	return this->display_list(
		layer,
		blend_mode,
		program
	).setup(&quad_allocator);
}
