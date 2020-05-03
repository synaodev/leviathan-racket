#include "./location.hpp"
#include "./kontext.hpp"

#include "../sym/renderer.hpp"

location_t::location_t() :
	position(0.0f),
	direction(direction_t::Right),
	bounding(0.0f, 0.0f, 16.0f, 16.0f)
{

}

location_t::location_t(glm::vec2 position) :
	position(position),
	direction(direction_t::Right),
	bounding(0.0f, 0.0f, 16.0f, 16.0f)
{

}

location_t::location_t(glm::vec2 position, direction_t direction) : 
	position(position),
	direction(direction),
	bounding(0.0f, 0.0f, 16.0f, 16.0f)
{

}

glm::vec2 location_t::center() const {
	return position + bounding.center();
}

rect_t location_t::hitbox() const {
	return rect_t(
		position + bounding.left_top(),
		bounding.dimensions()
	);
}

bool location_t::overlap(const location_t& that) const {
	return this->hitbox().overlaps(that.hitbox());
}

bool location_t::overlap(const rect_t& that) const {
	return this->hitbox().overlaps(that);
}

void location_t::hori(direction_t bits) {
	if (bits & direction_t::Left) {
		direction |= direction_t::Left;
	} else {
		direction &= ~direction_t::Left;
	}
}

void location_t::vert(direction_t bits) {
	if (bits & direction_t::Up) {
		direction |= direction_t::Up;
		direction &= ~direction_t::Down;
	} else if (bits & direction_t::Down) {
		direction |= direction_t::Down;
		direction &= ~direction_t::Up;
	} else {
		direction &= ~(direction_t::Up | direction_t::Down);
	}
}

void location_t::render(const kontext_t& kontext, renderer_t& renderer, rect_t viewport) {
	const glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
	auto& batch = renderer.get_normal_quads(
		layer_value::HeadsUp,
		blend_mode_t::Add,
		pipeline_t::VtxBlankColors
	);
	kontext.slice<location_t>().each([&batch, &viewport, &color](entt::entity, const location_t& location) {
		rect_t hitbox = location.hitbox();
		if (hitbox.overlaps(viewport)) {
			batch.begin(quad_batch_t::SingleQuad)
				.vtx_blank_write(hitbox, color)
				.vtx_transform_write(hitbox.left_top())
			.end();
		}
	});
}