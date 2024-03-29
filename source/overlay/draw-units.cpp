#include "./draw-units.hpp"

#include <glm/gtc/constants.hpp>

#include "../system/renderer.hpp"
#include "../video/texture.hpp"

namespace {
	constexpr arch_t kLoopPoint = 7;
}

draw_units_t::draw_units_t() {
	auto specify = vertex_spec_t::from(vtx_major_t::name());
	quads.setup(specify);
}

void draw_units_t::invalidate() const {
	amend = true;
}

void draw_units_t::render(renderer_t& renderer) const {
	if (!quads.empty()) {
		auto& list = renderer.display_list(
			layer_value::Persistent,
			blend_mode_t::Alpha,
			program_t::Sprites
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

void draw_units_t::set_position(real_t x, real_t y) {
	const glm::vec2 p { x, y };
	this->set_position(p);
}

void draw_units_t::set_position(const glm::vec2& position) {
	amend = true;
	this->position = position;
}

void draw_units_t::set_bounding(real_t x, real_t y, real_t w, real_t h) {
	const rect_t r { x, y, w, h };
	this->set_bounding(r);
}

void draw_units_t::set_bounding(const rect_t& bounding) {
	amend = true;
	this->bounding = bounding;
}

void draw_units_t::set_values(sint_t current, sint_t maximum) {
	this->set_values(current, maximum, tabular_value);
}

void draw_units_t::set_values(sint_t current, sint_t maximum, sint_t tabular) {
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
	if (this->tabular_value != tabular) {
		edited = true;
		this->tabular_value = tabular;
	}
	if (edited) {
		this->generate(
			static_cast<arch_t>(current),
			static_cast<arch_t>(maximum),
			resize
		);
	}
}

void draw_units_t::set_tabular(sint_t tabular) {
	if (this->tabular_value != tabular) {
		amend = true;
		this->set_values(current_value, maximum_value, tabular_value);
	}
}

void draw_units_t::set_texture(const texture_t* texture) {
	amend = true;
	this->texture = texture;
}

const glm::vec2& draw_units_t::get_position() const {
	return position;
}

void draw_units_t::generate(arch_t current, arch_t maximum, bool_t resize) {
	amend = true;
	if (resize) {
		quads.resize(maximum * display_list_t::SingleQuad);
	}
	const glm::vec2 offset { 0.0f, bounding.h * static_cast<real_t>(tabular_value) };
	glm::vec2 pos = position;
	glm::vec2 inv = texture ? texture->get_inverse_dimensions() : glm::one<glm::vec2>();
	sint_t texID = texture ? texture->get_name() : 0;
	for (arch_t it = 0, qindex = 0; it < maximum; ++it, ++qindex) {
		vtx_major_t* quad = quads.at<vtx_major_t>(qindex * display_list_t::SingleQuad);

		glm::vec2 uvs = bounding.left_top();
		if (current > 0 and it <= (current - 1)) {
			uvs.x += bounding.w;
		}

		quad[0].position = pos;
		quad[0].matrix = 0;
		quad[0].uvcoords = inv * (uvs + offset);
		quad[0].alpha = 1.0f;
		quad[0].texID = texID;

		quad[1].position = { pos.x, pos.y + bounding.h };
		quad[1].matrix = 0;
		quad[1].uvcoords = inv * glm::vec2(uvs.x + offset.x, uvs.y + bounding.h + offset.y);
		quad[1].alpha = 1.0f;
		quad[1].texID = texID;

		quad[2].position = { pos.x + bounding.w, pos.y };
		quad[2].matrix = 0;
		quad[2].uvcoords = inv * glm::vec2(uvs.x + bounding.w + offset.x, uvs.y + offset.y);
		quad[2].alpha = 1.0f;
		quad[2].texID = texID;

		quad[3].position = pos + bounding.dimensions();
		quad[3].matrix = 0;
		quad[3].uvcoords = inv * (uvs + bounding.dimensions() + offset);
		quad[3].alpha = 1.0f;
		quad[3].texID = texID;

		if (it != kLoopPoint) {
			pos.x += bounding.w;
		} else {
			pos.x = position.x;
			pos.y += bounding.h;
		}
	}
}
