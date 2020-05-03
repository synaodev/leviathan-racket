#include "./quad_batch.hpp"
#include "./program.hpp"

#include "../utl/rect.hpp"

quad_batch_t::quad_batch_t(layer_t layer, blend_mode_t blend_mode, const texture_t* texture, const palette_t* palette, const program_t* program) :
	layer(layer),
	blend_mode(blend_mode),
	texture(texture),
	palette(palette),
	program(program),
	drawn(false),
	write(false),
	current(0),
	account(0),
	quad_pool(),
	quad_list()
{
	vertex_spec_t specify;
	if (program != nullptr) {
		specify = program->get_specify();
	}
	quad_pool.setup(specify);
	quad_list.setup(buffer_usage_t::Dynamic, specify);
}

quad_batch_t::quad_batch_t() : 
	layer(layer_value::Automatic),
	blend_mode(blend_mode_t::None),
	texture(nullptr),
	palette(nullptr),
	program(nullptr),
	drawn(false),
	write(false),
	current(0),
	account(0),
	quad_pool(),
	quad_list()
{

}

quad_batch_t::quad_batch_t(quad_batch_t&& that) noexcept : quad_batch_t() {
	if (this != &that) {
		std::swap(layer, that.layer);
		std::swap(blend_mode, that.blend_mode);
		std::swap(texture, that.texture);
		std::swap(palette, that.palette);
		std::swap(program, that.program);
		std::swap(drawn, that.drawn);
		std::swap(write, that.write);
		std::swap(current, that.current);
		std::swap(account, that.account);
		std::swap(quad_pool, that.quad_pool);
		std::swap(quad_list, that.quad_list);
	}
}

quad_batch_t& quad_batch_t::operator=(quad_batch_t&& that) noexcept {
	if (this != &that) {
		std::swap(layer, that.layer);
		std::swap(blend_mode, that.blend_mode);
		std::swap(texture, that.texture);
		std::swap(palette, that.palette);
		std::swap(program, that.program);
		std::swap(drawn, that.drawn);
		std::swap(write, that.write);
		std::swap(current, that.current);
		std::swap(account, that.account);
		std::swap(quad_pool, that.quad_pool);
		std::swap(quad_list, that.quad_list);
	}
	return *this;
}

quad_batch_t& quad_batch_t::begin(arch_t count) {
	if ((current + count) > quad_pool.size()) {
		quad_pool.resize(current + count);
	}
	account = count;
	return *this;
}

quad_batch_t& quad_batch_t::vtx_pool_write(const vertex_pool_t* that_pool) {
#ifdef SYNAO_DEBUG_BUILD
	assert(that_pool != nullptr);
#endif // SYNAO_DEBUG_BUILD
	quad_pool.copy(current, account, that_pool);
	return *this;
}

quad_batch_t& quad_batch_t::vtx_major_write(rect_t texture_rect, glm::vec2 raster_dimensions, real_t table_index, real_t alpha_color, mirroring_t mirroring) {
	auto vtx = quad_pool.at<vtx_major_t>(current);
	vtx[0].position = glm::zero<glm::vec2>();
	vtx[0].uvcoords = texture_rect.left_top();
	vtx[0].table 	= table_index;
	vtx[0].alpha	= alpha_color;
	vtx[1].position = glm::vec2(0.0f, raster_dimensions.y);
	vtx[1].uvcoords = texture_rect.left_bottom();
	vtx[1].table 	= table_index;
	vtx[1].alpha	= alpha_color;
	vtx[2].position = glm::vec2(raster_dimensions.x, 0.0f);
	vtx[2].uvcoords = texture_rect.right_top();
	vtx[2].table 	= table_index;
	vtx[2].alpha	= alpha_color;
	vtx[3].position = raster_dimensions;
	vtx[3].uvcoords = texture_rect.right_bottom();
	vtx[3].table 	= table_index;
	vtx[3].alpha	= alpha_color;
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

quad_batch_t& quad_batch_t::vtx_blank_write(rect_t raster_rect, glm::vec4 vtx_color) {
	auto vtx = quad_pool.at<vtx_blank_t>(current);
	vtx[0].position = glm::zero<glm::vec2>();
	vtx[0].color 	= vtx_color;
	vtx[1].position = glm::vec2(0.0f, raster_rect.h);
	vtx[1].color 	= vtx_color;
	vtx[2].position = glm::vec2(raster_rect.w, 0.0f);
	vtx[2].color 	= vtx_color;
	vtx[3].position = raster_rect.dimensions();
	vtx[3].color 	= vtx_color;
	return *this;
}

quad_batch_t& quad_batch_t::vtx_transform_write(glm::vec2 position, glm::vec2 scale, glm::vec2 axis, real_t rotation) {
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

quad_batch_t& quad_batch_t::vtx_transform_write(glm::vec2 position, glm::vec2 axis, real_t rotation) {
	glm::vec2 scale = glm::one<glm::vec2>();
	return this->vtx_transform_write(position, scale, axis, rotation);
}

quad_batch_t& quad_batch_t::vtx_transform_write(glm::vec2 position, glm::vec2 scale) {
	vtx_minor_t* vtx = reinterpret_cast<vtx_minor_t*>(quad_pool[current]);
	for (arch_t it = 0; it < account; ++it) {
		vtx = reinterpret_cast<vtx_minor_t*>(quad_pool[current + it]);
		vtx->position *= scale;
		vtx->position += position;
	}
	return *this;
}

quad_batch_t& quad_batch_t::vtx_transform_write(glm::vec2 position) {
	glm::vec2 scale = glm::one<glm::vec2>();
	return this->vtx_transform_write(position, scale);
}

quad_batch_t& quad_batch_t::vtx_transform_write(real_t x, real_t y) {
	glm::vec2 position = glm::vec2(x, y);
	return this->vtx_transform_write(position);
}

void quad_batch_t::end() {
	write = true;
	current += account;
	account = 0;
}

void quad_batch_t::skip(arch_t count) {
	current += count;
	account = 0;
}

void quad_batch_t::skip() {
	current += account;
	account = 0;
}

void quad_batch_t::flush(gfx_t* gfx) {
#ifdef SYNAO_DEBUG_BUILD
	assert(gfx != nullptr);
	drawn = current != 0;
#endif // SYNAO_DEBUG_BUILD
	if (current != 0) {
		if (write) {
			write = false;
			if (current > quad_list.get_length()) {
				quad_list.create(current);
			}
			quad_list.update(quad_pool[0], current);
		}
		gfx->set_blend_mode(blend_mode);
		gfx->set_program(program);
		gfx->set_sampler(texture, 0);
		gfx->set_sampler(palette, 1);
		quad_list.draw(current);
	}
	current = 0;
}

bool quad_batch_t::matches(layer_t layer, blend_mode_t blend_mode, const texture_t* texture, const palette_t* palette, const program_t* program) const {
	return (
		layer_value::equal(this->layer, layer) and
		this->blend_mode == blend_mode and
		this->texture == texture and
		this->palette == palette and
		this->program == program
	);
}

bool quad_batch_t::visible() const {
	return drawn;
}

bool operator<(const quad_batch_t& lhv, const quad_batch_t& rhv) {
	if (layer_value::equal(lhv.layer, rhv.layer)) {
		if (lhv.blend_mode == rhv.blend_mode) {
			if (lhv.texture == rhv.texture) {
				if (lhv.palette == rhv.palette) {
					return lhv.program < rhv.program;
				}
				return lhv.palette < rhv.palette;
			}
			return lhv.texture < rhv.texture;
		}
		return lhv.blend_mode < rhv.blend_mode;
	}
	return lhv.layer < rhv.layer;
}