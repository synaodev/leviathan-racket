#include "./draw_count.hpp"

#include "../video/texture.hpp"
#include "../video/palette.hpp"
#include "../system/renderer.hpp"

static constexpr sint_t kFirst = -4735916;
static constexpr sint_t kRadix = 10;
static constexpr sint_t kMinus = 10;
static constexpr sint_t kPoint = 12;

draw_count_t::draw_count_t() :
	write(false),
	layer(layer_value::HeadsUp),
	backwards(false),
	visible(false),
	table(0.0f),
	position(0.0f),
	bounding(0.0f, 0.0f, 0.0f, 0.0f),
	value(kFirst),
	minimum_zeroes(0),
	texture(nullptr),
	palette(nullptr),
	quads()
{
	quads.setup<vtx_major_t>();
}

void draw_count_t::force() const {
	write = true;
}

void draw_count_t::render(renderer_t& renderer) const {
	if (visible and !quads.empty()) {
		auto& batch = renderer.get_overlay_quads(
			layer,
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

void draw_count_t::set_layer(layer_t layer) {
	write = true;
	this->layer = layer;
}

void draw_count_t::set_backwards(bool_t backwards) {
	write = true;
	this->backwards = backwards;
}

void draw_count_t::set_visible(bool_t visible) {
	write = true;
	this->visible = visible;
}

void draw_count_t::set_table(real_t table) {
	if (this->table != table) {
		write = true;
		this->table = table;
		this->new_value(value);
	}
}

void draw_count_t::set_position(real_t x, real_t y) {
	write = true;
	this->position = glm::vec2(x, y);
}

void draw_count_t::set_position(glm::vec2 position) {
	write = true;
	this->position = position;
}

void draw_count_t::mut_position(real_t x, real_t y) {
	write = true;
	this->position.x += x;
	this->position.y += y;
}

void draw_count_t::mut_position(glm::vec2 offset) {
	write = true;
	this->position += offset;
}

void draw_count_t::set_bounding(real_t x, real_t y, real_t w, real_t h) {
	write = true;
	this->bounding = rect_t(x, y, w, h);
}

void draw_count_t::set_bounding(rect_t bounding) {
	write = true;
	this->bounding = bounding;
}

void draw_count_t::new_value(sint_t number) {
	this->value = number;
	std::vector<sint_t> buffer;
	if (number < 0) {
		buffer.push_back(kMinus);
		number = -number;
	}
	do {
		buffer.push_back(number % kRadix);
		number /= kRadix;
	} while (number != 0);
	if (minimum_zeroes != 0) {
		for (arch_t e = value != 0 ? 0 : 1; e < minimum_zeroes; ++e) {
			if (value < quick_power_of_10(e)) {
				buffer.push_back(0);
			}
		}
	}
	this->generate_all(buffer);
}

void draw_count_t::set_value(sint_t number) {
	if (value != number) {
		this->new_value(number);
	}
}

void draw_count_t::add_value(sint_t number) {
	this->new_value(value + number);
}

void draw_count_t::set_minimum_zeroes(arch_t minimum_zeroes) {
	write = true;
	this->minimum_zeroes = minimum_zeroes;
}

void draw_count_t::set_texture(const texture_t* texture) {
	write = true;
	this->texture = texture;
}

void draw_count_t::set_palette(const palette_t* palette) {
	write = true;
	this->palette = palette;
}

glm::vec2 draw_count_t::get_position() const {
	return position;
}

bool draw_count_t::is_backwards() const {
	return backwards;
}

bool draw_count_t::is_visible() const {
	return visible;
}

sint_t draw_count_t::quick_power_of_10(arch_t exponent) {
	static const sint_t powers[10] = {
		1, 10, 100, 1000,
		10000, 100000, 1000000,
		10000000, 100000000, 1000000000
	};
	if (exponent < 10) {
		return powers[exponent];
	}
	return 0;
}

void draw_count_t::generate_all(const std::vector<sint_t>& buffer) {
	write = true;
	if (buffer.size() != quads.size() / quad_batch_t::SingleQuad) {
		quads.resize(buffer.size() * quad_batch_t::SingleQuad);
	}
	glm::vec2 pos = position;
	glm::vec2 inv = texture->get_inverse_dimensions();
	arch_t qindex = 0;
	if (backwards) {
		for (auto it = buffer.begin(); it != buffer.end(); ++it, ++qindex) {
			glm::vec2 txcd = glm::vec2(
				bounding.x + static_cast<real_t>(*it) * bounding.w,
				bounding.y
			);
			this->generate_one(
				quads.at<vtx_major_t>(qindex * quad_batch_t::SingleQuad), 
				pos, txcd, inv
			);
			pos.x -= (*it == kPoint) ? 
				bounding.w / 2.0f : 
				bounding.w;
		}
	} else {
		for (auto it = buffer.rbegin(); it != buffer.rend(); ++it, ++qindex) {
			glm::vec2 txcd = glm::vec2(
				bounding.x + static_cast<real_t>(*it) * bounding.w,
				bounding.y
			);
			this->generate_one(
				quads.at<vtx_major_t>(qindex * quad_batch_t::SingleQuad), 
				pos, txcd, inv
			);
			pos.x += (*it == kPoint) ? 
				bounding.w / 2.0f : 
				bounding.w;
		}
	}
}

void draw_count_t::generate_one(vtx_major_t* quad, glm::vec2 pos, glm::vec2 uvs, glm::vec2 inv) {
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
}