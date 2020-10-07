#include "./display-list.hpp"
#include "./pipeline.hpp"

#include "../utility/watch.hpp"
#include "../utility/rect.hpp"

display_list_t::display_list_t(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, const pipeline_t* pipeline, const quad_allocator_t* allocator) :
	layer(layer),
	blend_mode(blend_mode),
	pipeline(pipeline),
	visible(false),
	amend(false),
	current(0),
	account(0),
	quad_pool(),
	quad_buffer()
{
	vertex_spec_t specify;
	if (pipeline != nullptr) {
		specify = pipeline->get_specify();
	}
	quad_pool.setup(specify);
	quad_buffer.setup(allocator, usage, specify);
}

display_list_t::display_list_t() :
	layer(layer_value::Automatic),
	blend_mode(blend_mode_t::None),
	pipeline(nullptr),
	visible(false),
	amend(false),
	current(0),
	account(0),
	quad_pool(),
	quad_buffer()
{

}

display_list_t::display_list_t(display_list_t&& that) noexcept : display_list_t() {
	if (this != &that) {
		std::swap(layer, that.layer);
		std::swap(blend_mode, that.blend_mode);
		std::swap(pipeline, that.pipeline);
		std::swap(visible, that.visible);
		std::swap(amend, that.amend);
		std::swap(current, that.current);
		std::swap(account, that.account);
		std::swap(quad_pool, that.quad_pool);
		std::swap(quad_buffer, that.quad_buffer);
	}
}

display_list_t& display_list_t::operator=(display_list_t&& that) noexcept {
	if (this != &that) {
		std::swap(layer, that.layer);
		std::swap(blend_mode, that.blend_mode);
		std::swap(pipeline, that.pipeline);
		std::swap(visible, that.visible);
		std::swap(amend, that.amend);
		std::swap(current, that.current);
		std::swap(account, that.account);
		std::swap(quad_pool, that.quad_pool);
		std::swap(quad_buffer, that.quad_buffer);
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

display_list_t& display_list_t::vtx_blank_write(rect_t raster_rect, glm::vec4 vtx_color) {
	const sint_t matrix = layer > layer_value::TileFront ? 0 : 1;
	auto vtx = quad_pool.at<vtx_blank_t>(current);
	vtx[0].position = glm::zero<glm::vec2>();
	vtx[0].matrix 	= matrix;
	vtx[0].color 	= vtx_color;
	vtx[1].position = glm::vec2(0.0f, raster_rect.h);
	vtx[1].matrix 	= matrix;
	vtx[1].color 	= vtx_color;
	vtx[2].position = glm::vec2(raster_rect.w, 0.0f);
	vtx[2].matrix 	= matrix;
	vtx[2].color 	= vtx_color;
	vtx[3].position = raster_rect.dimensions();
	vtx[3].matrix 	= matrix;
	vtx[3].color 	= vtx_color;
	return *this;
}

display_list_t& display_list_t::vtx_major_write(rect_t texture_rect, glm::vec2 raster_dimensions, mirroring_t mirroring, real_t alpha_color, sint_t texture_name, sint_t palette_name) {
	const sint_t matrix = layer > layer_value::TileFront ? 0 : 1;
	auto vtx = quad_pool.at<vtx_major_t>(current);
	vtx[0].position = glm::zero<glm::vec2>();
	vtx[0].matrix 	= matrix;
	vtx[0].uvcoords = texture_rect.left_top();
	vtx[0].alpha	= alpha_color;
	vtx[0].texID 	= texture_name;
	vtx[0].palID 	= palette_name;
	vtx[1].position = glm::vec2(0.0f, raster_dimensions.y);
	vtx[1].matrix 	= matrix;
	vtx[1].uvcoords = texture_rect.left_bottom();
	vtx[1].alpha	= alpha_color;
	vtx[1].texID 	= texture_name;
	vtx[1].palID 	= palette_name;
	vtx[2].position = glm::vec2(raster_dimensions.x, 0.0f);
	vtx[2].matrix 	= matrix;
	vtx[2].uvcoords = texture_rect.right_top();
	vtx[2].alpha	= alpha_color;
	vtx[2].texID 	= texture_name;
	vtx[2].palID 	= palette_name;
	vtx[3].position = raster_dimensions;
	vtx[3].matrix 	= matrix;
	vtx[3].uvcoords = texture_rect.right_bottom();
	vtx[3].alpha	= alpha_color;
	vtx[3].texID 	= texture_name;
	vtx[3].palID 	= palette_name;
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

display_list_t& display_list_t::vtx_fonts_write(rect_t texture_rect, glm::vec2 raster_dimensions, glm::vec4 full_color, sint_t texture_name, sint_t table_name) {
	const sint_t matrix = layer > layer_value::TileFront ? 0 : 1;
	auto vtx = quad_pool.at<vtx_fonts_t>(current);
	vtx[0].position = glm::zero<glm::vec2>();
	vtx[0].matrix = matrix;
	vtx[0].uvcoords = texture_rect.left_top();
	vtx[0].color = full_color;
	vtx[0].texID = texture_name;
	vtx[0].tblID = table_name;
	vtx[1].position = glm::vec2(0.0f, raster_dimensions.y);
	vtx[1].matrix = matrix;
	vtx[1].uvcoords = texture_rect.left_bottom();
	vtx[1].color = full_color;
	vtx[1].texID = texture_name;
	vtx[1].tblID = table_name;
	vtx[2].position = glm::vec2(raster_dimensions.x, 0.0f);
	vtx[2].matrix = matrix;
	vtx[2].uvcoords = texture_rect.right_top();
	vtx[2].color = full_color;
	vtx[2].texID = texture_name;
	vtx[2].tblID = table_name;
	vtx[3].position = raster_dimensions;
	vtx[3].matrix = matrix;
	vtx[3].uvcoords = texture_rect.right_bottom();
	vtx[3].color = full_color;
	vtx[3].texID = texture_name;
	vtx[3].tblID = table_name;
	return *this;
}

display_list_t& display_list_t::vtx_transform_write(glm::vec2 position, glm::vec2 scale, glm::vec2 axis, real_t rotation) {
	auto vtx = reinterpret_cast<vtx_minor_t*>(quad_pool[current]);
	glm::vec2 left_top = position + (scale * vtx->position);
	real_t cos = rotation != 0.0f ? glm::cos(rotation) : 1.0f;
	real_t sin = rotation != 0.0f ? glm::sin(rotation) : 0.0f;
	for (arch_t it = 0; it < account; ++it) {
		vtx = reinterpret_cast<vtx_minor_t*>(quad_pool[current + it]);
		glm::vec2 beg_pos = (position + (scale * vtx->position)) - left_top - axis;
		glm::vec2 end_pos  = glm::vec2(
			beg_pos.x * cos - beg_pos.y * sin,
			beg_pos.x * sin + beg_pos.y * cos
		);
		vtx->position = end_pos + left_top + axis;
	}
	return *this;
}

display_list_t& display_list_t::vtx_transform_write(glm::vec2 position, glm::vec2 axis, real_t rotation) {
	glm::vec2 scale = glm::one<glm::vec2>();
	return this->vtx_transform_write(position, scale, axis, rotation);
}

display_list_t& display_list_t::vtx_transform_write(glm::vec2 position, glm::vec2 scale) {
	vtx_minor_t* vtx = reinterpret_cast<vtx_minor_t*>(quad_pool[current]);
	for (arch_t it = 0; it < account; ++it) {
		vtx = reinterpret_cast<vtx_minor_t*>(quad_pool[current + it]);
		vtx->position *= scale;
		vtx->position += position;
	}
	return *this;
}

display_list_t& display_list_t::vtx_transform_write(glm::vec2 position) {
	glm::vec2 scale = glm::one<glm::vec2>();
	return this->vtx_transform_write(position, scale);
}

display_list_t& display_list_t::vtx_transform_write(real_t x, real_t y) {
	glm::vec2 position = glm::vec2(x, y);
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

bool display_list_t::matches(layer_t layer, blend_mode_t blend_mode, buffer_usage_t usage, const pipeline_t* pipeline) const {
	return (
		layer_value::equal(this->layer, layer) and
		this->blend_mode == blend_mode and
		this->quad_buffer.get_usage() == usage and
		this->pipeline == pipeline
	);
}

bool display_list_t::rendered() const {
	return visible;
}

bool operator<(const display_list_t& lhv, const display_list_t& rhv) {
	if (layer_value::equal(lhv.layer, rhv.layer)) {
		if (lhv.blend_mode == rhv.blend_mode) {
			buffer_usage_t lhu = lhv.quad_buffer.get_usage();
			buffer_usage_t rhu = rhv.quad_buffer.get_usage();
			if (lhu == rhu) {
				return lhv.pipeline < rhv.pipeline;
			}
			return lhu < rhu;
		}
		return lhv.blend_mode < rhv.blend_mode;
	}
	return lhv.layer < rhv.layer;
}
