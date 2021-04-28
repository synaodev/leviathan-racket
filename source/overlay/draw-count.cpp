#include "./draw-count.hpp"

#include <glm/exponential.hpp>
#include <glm/gtc/constants.hpp>

#include "../video/texture.hpp"
#include "../system/renderer.hpp"

static constexpr sint_t kRadix = 10;
static constexpr sint_t kMinus = 10;
static constexpr sint_t kPoint = 12;

draw_count_t::draw_count_t() {
	auto specify = vertex_spec_t::from(vtx_major_t::name());
	quads.setup(specify);
}

void draw_count_t::invalidate() const {
	amend = true;
}

void draw_count_t::render(renderer_t& renderer) const {
	if (visible and !quads.empty()) {
		auto& list = renderer.display_list(
			layer,
			blend_mode_t::Alpha,
			program_t::Indexed
		);
		if (amend) {
			amend = false;
			list.begin(quads.size())
				.vtx_pool_write(quads)
			.end();
		} else {
			list.skip(quads.size());
		}
	}
}

void draw_count_t::set_layer(layer_t layer) {
	amend = true;
	this->layer = layer;
}

void draw_count_t::set_backwards(bool_t backwards) {
	amend = true;
	this->backwards = backwards;
}

void draw_count_t::set_visible(bool_t visible) {
	amend = true;
	this->visible = visible;
}

void draw_count_t::set_table(sint_t table) {
	if (this->table != table) {
		amend = true;
		this->table = table;
		this->new_value(value);
	}
}

void draw_count_t::set_position(real_t x, real_t y) {
	const glm::vec2 p { x, y };
	this->set_position(p);
}

void draw_count_t::set_position(const glm::vec2& position) {
	amend = true;
	this->position = position;
}

void draw_count_t::mut_position(real_t x, real_t y) {
	const glm::vec2 p { x, y };
	this->mut_position(p);
}

void draw_count_t::mut_position(const glm::vec2& offset) {
	amend = true;
	this->position += offset;
}

void draw_count_t::set_bounding(real_t x, real_t y, real_t w, real_t h) {
	const rect_t r { x, y, w, h };
	this->set_bounding(r);
}

void draw_count_t::set_bounding(const rect_t& bounding) {
	amend = true;
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
		for (sint_t e = value != 0 ? 0 : 1; e < minimum_zeroes; ++e) {
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
	amend = true;
	this->minimum_zeroes = minimum_zeroes;
}

void draw_count_t::set_texture(const texture_t* texture) {
	amend = true;
	this->texture = texture;
}

void draw_count_t::set_palette(const palette_t* palette) {
	amend = true;
	this->palette = palette;
}

const glm::vec2& draw_count_t::get_position() const {
	return position;
}

bool draw_count_t::is_backwards() const {
	return backwards;
}

bool draw_count_t::is_visible() const {
	return visible;
}

sint_t draw_count_t::quick_power_of_10(sint_t exponent) {
	static const sint_t powers[10] = {
		1, 10, 100, 1000,
		10000, 100000, 1000000,
		10000000, 100000000, 1000000000
	};
	if (exponent >= 0 and exponent < 10) {
		return powers[exponent];
	}
	return static_cast<sint_t>(glm::pow(10, exponent));
}

void draw_count_t::generate_all(const std::vector<sint_t>& buffer) {
	amend = true;
	if (buffer.size() != (quads.size() / display_list_t::SingleQuad)) {
		quads.resize(buffer.size() * display_list_t::SingleQuad);
	}
	glm::vec2 pos = position;
	glm::vec2 inv = texture ? texture->get_inverse_dimensions() : glm::one<glm::vec2>();
	sint_t texID = texture ? texture->get_name() : 0;
	sint_t palID = palette ? palette->get_name() + table : 0;
	arch_t qindex = 0;
	if (backwards) {
		for (auto it = buffer.begin(); it != buffer.end(); ++it, ++qindex) {
			const glm::vec2 txcd {
				bounding.x + static_cast<real_t>(*it) * bounding.w,
				bounding.y
			};
			this->generate_one(
				quads.at<vtx_major_t>(qindex * display_list_t::SingleQuad),
				pos, txcd, inv, texID, palID
			);
			pos.x -= (*it == kPoint) ?
				bounding.w / 2.0f :
				bounding.w;
		}
	} else {
		for (auto it = buffer.rbegin(); it != buffer.rend(); ++it, ++qindex) {
			const glm::vec2 txcd {
				bounding.x + static_cast<real_t>(*it) * bounding.w,
				bounding.y
			};
			this->generate_one(
				quads.at<vtx_major_t>(qindex * display_list_t::SingleQuad),
				pos, txcd, inv, texID, palID
			);
			pos.x += (*it == kPoint) ?
				bounding.w / 2.0f :
				bounding.w;
		}
	}
}

void draw_count_t::generate_one(vtx_major_t* quad, glm::vec2 pos, glm::vec2 uvs, glm::vec2 inv, sint_t texID, sint_t palID) {
	quad[0].position = pos;
	quad[0].matrix = 0;
	quad[0].uvcoords = uvs * inv;
	quad[0].alpha = 1.0f;
	quad[0].texID = texID;
	quad[0].palID = palID;

	quad[1].position = { pos.x, pos.y + bounding.h };
	quad[1].matrix = 0;
	quad[1].uvcoords = glm::vec2(uvs.x, uvs.y + bounding.h) * inv;
	quad[1].alpha = 1.0f;
	quad[1].texID = texID;
	quad[1].palID = palID;

	quad[2].position = { pos.x + bounding.w, pos.y };
	quad[2].matrix = 0;
	quad[2].uvcoords = glm::vec2(uvs.x + bounding.w, uvs.y) * inv;
	quad[2].alpha = 1.0f;
	quad[2].texID = texID;
	quad[2].palID = palID;

	quad[3].position = pos + bounding.dimensions();
	quad[3].matrix = 0;
	quad[3].uvcoords = inv * (uvs + bounding.dimensions());
	quad[3].alpha = 1.0f;
	quad[3].texID = texID;
	quad[3].palID = palID;
}
