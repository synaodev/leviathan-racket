#include "./draw_units.hpp"

#include "../sys/renderer.hpp"
#include "../gfx/texture.hpp"

static constexpr arch_t kLoopPoint = 7;

draw_units_t::draw_units_t() :
	write(false),
	position(0.0f),
	bounding(0.0f, 0.0f, 0.0f, 0.0f),
	current_value(0),
	maximum_value(0),
	table(0.0f),
	texture(nullptr),
	quads()
{
	quads.setup<vtx_major_t>();
}

void draw_units_t::force() const {
	write = true;
}

void draw_units_t::render(renderer_t& renderer) const {
	if (!quads.empty()) {
		auto& batch = renderer.get_overlay_quads(
			layer_value::HeadsUp,
			blend_mode_t::Alpha, 
			pipeline_t::VtxMajorIndexed,
			texture,
			palette
		);
		if (write) {
			write = false;
			batch.begin(quads.size())
				.vtx_pool_write(&quads)
			.end();
		} else {
			batch.skip(quads.size());
		}
	}
}

void draw_units_t::set_position(real_t x, real_t y) {
	write = true;
	this->position = glm::vec2(x, y);
}

void draw_units_t::set_position(glm::vec2 position) {
	write = true;
	this->position = position;
}

void draw_units_t::set_bounding(real_t x, real_t y, real_t w, real_t h) {
	write = true;
	this->bounding = rect_t(x, y, w, h);
}

void draw_units_t::set_bounding(rect_t bounding) {
	write = true;
	this->bounding = bounding;
}

void draw_units_t::set_values(sint_t current, sint_t maximum) {
	bool_t edited = false;
	bool_t resize = false;
	if (this->maximum_value != maximum) {
		edited = true;
		resize = true;
		this->maximum_value = maximum;
	}
	if (this->current_value != current) {
		edited = true;
		this->current_value = current;
	}
	if (edited) {
		this->generate(
			static_cast<arch_t>(current),
			static_cast<arch_t>(maximum),
			resize
		);
	}
}

void draw_units_t::set_table(real_t table) {
	if (this->table != table) {
		write = true;
		this->table = table;
		this->set_values(current_value, maximum_value);
	}	
}

void draw_units_t::set_texture(const texture_t* texture) {
	write = true;
	this->texture = texture;
}

void draw_units_t::set_palette(const palette_t* palette) {
	write = true;
	this->palette = palette;
}

glm::vec2 draw_units_t::get_position() const {
	return position;
}

void draw_units_t::generate(arch_t current, arch_t maximum, bool_t resize) {
	write = true;
	if (resize) {
		quads.resize(maximum * quad_batch_t::SingleQuad);
	}
	glm::vec2 pos = position;
	glm::vec2 inv = texture->get_inverse_dimensions();
	for (arch_t it = 0, qindex = 0; it < maximum; ++it, ++qindex) {
		vtx_major_t* quad = quads.at<vtx_major_t>(qindex * quad_batch_t::SingleQuad);

		glm::vec2 uvs = bounding.left_top();
		if (current > 0 and it <= current - 1) {
			uvs.x += bounding.w;
		}

		quad[0].position = pos;
		quad[0].uvcoords = uvs * inv;
		quad[0].table = table;
		quad[0].alpha = 1.0f;

		quad[1].position = glm::vec2(pos.x, pos.y + bounding.h);
		quad[1].uvcoords = glm::vec2(uvs.x, uvs.y + bounding.h) * inv;
		quad[1].table = table;
		quad[1].alpha = 1.0f;

		quad[2].position = glm::vec2(pos.x + bounding.w, pos.y);
		quad[2].uvcoords = glm::vec2(uvs.x + bounding.w, uvs.y) * inv;
		quad[2].table = table;
		quad[2].alpha = 1.0f;

		quad[3].position = pos + bounding.dimensions();
		quad[3].uvcoords = inv * (uvs + bounding.dimensions());
		quad[3].table = table;
		quad[3].alpha = 1.0f;

		if (it != kLoopPoint) {	
			pos.x += bounding.w;
		} else {
			pos.x = position.x;
			pos.y += bounding.h;
		}
	}
}