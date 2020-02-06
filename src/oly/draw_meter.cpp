#include "./draw_meter.hpp"

#include "../cnt/kontext.hpp"
#include "../cnt/health.hpp"
#include "../sys/renderer.hpp"

static const glm::vec2 kGraphedPosition		= glm::vec2(302.0f, 142.0f);
static const glm::vec2 kVaryingPosition 	= kGraphedPosition + glm::vec2(9.0f, 1.0f);
static const glm::vec2 kVaryingDimensions 	= glm::vec2(6.0f, 94.0f);

draw_meter_t::draw_meter_t() :
	write(false),
	current(0),
	identity(0),
	varying(kGraphedPosition, kVaryingDimensions),
	graphed()
{
	
}

void draw_meter_t::init(const animation_t* animation) {
	this->reset();
	graphed.set_file(animation);
	graphed.set_state(6);
	graphed.set_position(kVaryingPosition);
}

void draw_meter_t::reset() {
	this->force();
	current = 0;
	identity = 0;
}

void draw_meter_t::handle(const kontext_t& kontext) {
	if (identity != 0) {
		entt::entity actor = kontext.search_id(identity);
		if (actor != entt::null) {
			if (kontext.has<health_t>(actor)) {
				auto& health = kontext.get<health_t>(actor);
				if (this->current != health.current) {
					this->current = health.current;
					if (health.maximum != 0) {
						real_t ratio = static_cast<real_t>(health.current) / static_cast<real_t>(health.maximum);
						varying.h = glm::round(ratio * kVaryingDimensions.y);
						varying.y = kVaryingPosition.y + varying.h;
					} else {
						varying.h = 0.0f;
					}
					this->force();
				}
			} else {
				identity = 0;
			}
		} else {
			identity = 0;
		}
	}
}

void draw_meter_t::update(real64_t delta) {
	if (identity != 0) {
		graphed.update(delta);
	}
}

void draw_meter_t::force() const {
	write = true;
	graphed.force();
}

void draw_meter_t::render(renderer_t& renderer) const {
	if (identity != 0) {
		graphed.render(renderer);
		auto& batch = renderer.get_overlay_quads(
			layer_value::HeadsUp,
			blend_mode_t::Alpha,
			pipeline_t::VtxBlankColors,
			nullptr,
			nullptr
		);
		if (write) {
			write = false;
			batch.begin(quad_batch_t::SingleQuad)
				.vtx_blank_write(varying, glm::one<glm::vec4>())
				.vtx_transform_write(varying.left_top())
			.end();
		} else {
			batch.skip(quad_batch_t::SingleQuad);
		}
	}
}

void draw_meter_t::set_enemy(sint_t identity) {
	if (identity > 0) {
		this->force();
		current = 0;
		this->identity = identity;
	} else {
		this->reset();
	}
}

bool draw_meter_t::is_active() const {
	return identity != 0;
}
