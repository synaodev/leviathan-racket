#include "./display-list.hpp"
#include "./pipeline.hpp"

#include "../utility/watch.hpp"
#include "../utility/rect.hpp"

#include <glm/trigonometric.hpp>
#include <glm/gtc/constants.hpp>

bool display_list_t::operator<(const display_list_t& that) {
	if (layer_value::equal(this->layer, that.layer)) {
		if (this->blend_mode == that.blend_mode) {
			return this->pipeline < that.pipeline;
		}
		return this->blend_mode < that.blend_mode;
	}
	return this->layer < that.layer;
}

display_list_t& display_list_t::setup(const quad_allocator_t* allocator) {
	if (!quad_buffer.valid()) {
		vertex_spec_t specify;
		if (pipeline) {
			specify = pipeline->get_specify();
		}
		quad_pool.setup(specify);
		quad_buffer.setup(allocator, buffer_usage_t::Dynamic, specify);
	}
	return *this;
}

display_list_t& display_list_t::begin(arch_t count) {
	if ((current + count) > quad_pool.size()) {
		quad_pool.resize(current + count);
	}
	account = count;
	return *this;
}

display_list_t& display_list_t::vtx_pool_write(const vertex_pool_t& that_pool) {
	quad_pool.copy(current, account, that_pool);
	return *this;
}

display_list_t& display_list_t::vtx_blank_write(const rect_t& raster_rect, const glm::vec4& vtx_color) {
	const sint_t matrix = layer == layer_value::Persistent ? 0 : 1;
	auto vtx = quad_pool.at<vtx_blank_t>(current);
	vtx[0].position = glm::zero<glm::vec2>();
	vtx[0].matrix 	= matrix;
	vtx[0].color 	= vtx_color;
	vtx[1].position = { 0.0f, raster_rect.h };
	vtx[1].matrix 	= matrix;
	vtx[1].color 	= vtx_color;
	vtx[2].position = { raster_rect.w, 0.0f };
	vtx[2].matrix 	= matrix;
	vtx[2].color 	= vtx_color;
	vtx[3].position = raster_rect.dimensions();
	vtx[3].matrix 	= matrix;
	vtx[3].color 	= vtx_color;
	return *this;
}

display_list_t& display_list_t::vtx_major_write(const rect_t& texture_rect, const glm::vec2& raster_dimensions, mirroring_t mirroring, real_t alpha_color, sint_t texture_name) {
	const sint_t matrix = layer == layer_value::Persistent ? 0 : 1;
	auto vtx = quad_pool.at<vtx_major_t>(current);
	vtx[0].position = glm::zero<glm::vec2>();
	vtx[0].matrix 	= matrix;
	vtx[0].uvcoords = texture_rect.left_top();
	vtx[0].alpha	= alpha_color;
	vtx[0].texID 	= texture_name;
	vtx[1].position = { 0.0f, raster_dimensions.y };
	vtx[1].matrix 	= matrix;
	vtx[1].uvcoords = texture_rect.left_bottom();
	vtx[1].alpha	= alpha_color;
	vtx[1].texID 	= texture_name;
	vtx[2].position = { raster_dimensions.x, 0.0f };
	vtx[2].matrix 	= matrix;
	vtx[2].uvcoords = texture_rect.right_top();
	vtx[2].alpha	= alpha_color;
	vtx[2].texID 	= texture_name;
	vtx[3].position = raster_dimensions;
	vtx[3].matrix 	= matrix;
	vtx[3].uvcoords = texture_rect.right_bottom();
	vtx[3].alpha	= alpha_color;
	vtx[3].texID 	= texture_name;
	switch (mirroring) {
	case mirroring_t::None:
		break;
	case mirroring_t::Horizontal:
		std::swap(vtx[0].uvcoords.x, vtx[3].uvcoords.x);
		std::swap(vtx[1].uvcoords.x, vtx[2].uvcoords.x);
		break;
	case mirroring_t::Vertical:
		std::swap(vtx[0].uvcoords.y, vtx[3].uvcoords.y);
		std::swap(vtx[1].uvcoords.y, vtx[2].uvcoords.y);
		break;
	case mirroring_t::Both:
		std::swap(vtx[0].uvcoords, vtx[3].uvcoords);
		std::swap(vtx[1].uvcoords, vtx[2].uvcoords);
		break;
	}
	return *this;
}

display_list_t& display_list_t::vtx_fonts_write(const rect_t& texture_rect, const glm::vec2& raster_dimensions, const glm::vec4& full_color, sint_t atlas_name, sint_t atlas_table) {
	auto vtx = quad_pool.at<vtx_fonts_t>(current);
	vtx[0].position = glm::zero<glm::vec2>();
	vtx[0].uvcoords = texture_rect.left_top();
	vtx[0].color = full_color;
	vtx[0].atlas = atlas_name;
	vtx[0].table = atlas_table;
	vtx[1].position = { 0.0f, raster_dimensions.y };
	vtx[1].uvcoords = texture_rect.left_bottom();
	vtx[1].color = full_color;
	vtx[1].atlas = atlas_name;
	vtx[1].table = atlas_table;
	vtx[2].position = { raster_dimensions.x, 0.0f };
	vtx[2].uvcoords = texture_rect.right_top();
	vtx[2].color = full_color;
	vtx[2].atlas = atlas_name;
	vtx[2].table = atlas_table;
	vtx[3].position = raster_dimensions;
	vtx[3].uvcoords = texture_rect.right_bottom();
	vtx[3].color = full_color;
	vtx[3].atlas = atlas_name;
	vtx[3].table = atlas_table;
	return *this;
}

display_list_t& display_list_t::vtx_transform_write(const glm::vec2& position, const glm::vec2& scale, const glm::vec2& axis, real_t rotation) {
	auto vtx = reinterpret_cast<vtx_minor_t*>(quad_pool[current]);
	glm::vec2 left_top = position + (scale * vtx->position);
	real_t cos = rotation != 0.0f ? glm::cos(rotation) : 1.0f;
	real_t sin = rotation != 0.0f ? glm::sin(rotation) : 0.0f;
	for (arch_t it = 0; it < account; ++it) {
		vtx = reinterpret_cast<vtx_minor_t*>(quad_pool[current + it]);
		glm::vec2 beg_pos = (position + (scale * vtx->position)) - left_top - axis;
		glm::vec2 end_pos  = {
			beg_pos.x * cos - beg_pos.y * sin,
			beg_pos.x * sin + beg_pos.y * cos
		};
		vtx->position = end_pos + left_top + axis;
	}
	return *this;
}

display_list_t& display_list_t::vtx_transform_write(const glm::vec2& position, const glm::vec2& axis, real_t rotation) {
	const glm::vec2 scale = glm::one<glm::vec2>();
	return this->vtx_transform_write(position, scale, axis, rotation);
}

display_list_t& display_list_t::vtx_transform_write(const glm::vec2& position, const glm::vec2& scale) {
	vtx_minor_t* vtx = reinterpret_cast<vtx_minor_t*>(quad_pool[current]);
	for (arch_t it = 0; it < account; ++it) {
		vtx = reinterpret_cast<vtx_minor_t*>(quad_pool[current + it]);
		vtx->position *= scale;
		vtx->position += position;
	}
	return *this;
}

display_list_t& display_list_t::vtx_transform_write(const glm::vec2& position) {
	const glm::vec2 scale = glm::one<glm::vec2>();
	return this->vtx_transform_write(position, scale);
}

display_list_t& display_list_t::vtx_transform_write(real_t x, real_t y) {
	const glm::vec2 position = { x, y };
	return this->vtx_transform_write(position);
}

void display_list_t::end() {
	amend = true;
	current += account;
	account = 0;
}

void display_list_t::skip(arch_t count) {
	current += count;
	account = 0;
}

void display_list_t::skip() {
	current += account;
	account = 0;
}

void display_list_t::flush(gfx_t& gfx, const sampler_allocator_t* samplers) {
	visible = current != 0;
	if (visible) {
		if (amend) {
			amend = false;
			if (current > quad_buffer.get_length()) {
				quad_buffer.create(current);
			}
			quad_buffer.update(quad_pool[0], current);
		}
		gfx.set_blend_mode(blend_mode);
		gfx.set_pipeline(pipeline);
		gfx.set_sampler_allocator(samplers);
		quad_buffer.draw(current);
	}
	current = 0;
}

bool display_list_t::matches(layer_t layer, blend_mode_t blend_mode, const pipeline_t* pipeline) const {
	return (
		layer_value::equal(this->layer, layer) and
		this->blend_mode == blend_mode and
		this->pipeline == pipeline
	);
}

bool display_list_t::rendered() const {
	return visible;
}
