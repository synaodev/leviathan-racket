#include "./draw-meter.hpp"

#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>

#include "../system/renderer.hpp"

static const glm::vec2 kGraphedPosition		= glm::vec2(302.0f, 142.0f);
static const glm::vec2 kVaryingPosition 	= kGraphedPosition + glm::vec2(9.0f, 1.0f);
static const glm::vec2 kVaryingDimensions 	= glm::vec2(6.0f, 94.0f);

draw_meter_t::draw_meter_t() :
	amend(false),
	current(0),
	varying(kGraphedPosition, kVaryingDimensions),
	graphed()
{

}

void draw_meter_t::init(const animation_t* animation) {
	this->reset();
	graphed.set_file(animation);
	graphed.set_state(3);
	graphed.set_position(kVaryingPosition);
}

void draw_meter_t::reset() {
	this->invalidate();
	current = 0;
}

void draw_meter_t::update(real64_t delta) {
	if (current != 0) {
		graphed.update(delta);
	}
}

void draw_meter_t::invalidate() const {
	amend = true;
	graphed.invalidate();
}

void draw_meter_t::render(renderer_t& renderer) const {
	if (current != 0) {
		graphed.render(renderer);
		auto& list = renderer.display_list(
			layer_value::Persistent,
			blend_mode_t::Alpha,
			program_t::Colors
		);
		if (amend) {
			amend = false;
			list.begin(display_list_t::SingleQuad)
				.vtx_blank_write(varying, glm::one<glm::vec4>())
				.vtx_transform_write(varying.left_top())
			.end();
		} else {
			list.skip(display_list_t::SingleQuad);
		}
	}
}

void draw_meter_t::set_values(sint_t current, sint_t maximum) {
	if (current == 0) {
		this->reset();
	} else if (this->current != current) {
		this->current = current;
		if (maximum != 0) {
			real_t ratio = static_cast<real_t>(current) / static_cast<real_t>(maximum);
			varying.h = glm::round(ratio * kVaryingDimensions.y);
			varying.y = kVaryingPosition.y + varying.h;
		} else {
			varying.h = 0.0f;
		}
		this->invalidate();
	}
}
