#include "./sprite.hpp"
#include "./location.hpp"
#include "./kontext.hpp"

#include "../resource/vfs.hpp"
#include "../utility/logger.hpp"

#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>

sprite_t::sprite_t(const entt::hashed_string& entry) {
	file = vfs_t::animation(entry);
}

void sprite_t::reset() {
	timer = 0.0;
	alpha = 1.0f;
	state = 0;
	variation = 0;
	mirroring = mirroring_t::None;
	frame = 0;
	layer = layer_value::Automatic;
	scale = glm::one<glm::vec2>();
	pivot = glm::zero<glm::vec2>();
	angle = 0.0f;
	shake = 0.0f;
}

void sprite_t::new_state(arch_t state) {
	if (state != NonState) {
		if (this->state != state) {
			this->timer = 0.0;
			this->state = state;
			this->frame = 0;
		}
	}
}

glm::vec2 sprite_t::action_point(arch_t state, arch_t variation, mirroring_t mirroring, const glm::vec2& position) const {
	if (file) {
		return position + file->get_action_point(state, variation, mirroring);
	}
	return position;
}

bool sprite_t::finished() const {
	if (file) {
		return file->is_finished(state, frame, timer);
	}
	return true;
}

void sprite_t::update(kontext_t& kontext, real64_t delta) {
	kontext.slice<sprite_t>().each([delta](entt::entity, sprite_t& sprite) {
		if (sprite.file) {
			sprite.file->update(
				delta,
				sprite.state,
				sprite.timer,
				sprite.frame
			);
			if (sprite.shake != 0.0f) {
				real_t amount = static_cast<real_t>(delta / 2.0f);
				sprite.shake = sprite.shake > 0.0f ?
					sprite.shake = -glm::max(0.0f, sprite.shake - amount) :
					sprite.shake = -glm::min(0.0f, sprite.shake + amount);
			}
		}
	});
}

void sprite_t::render(const kontext_t& kontext, renderer_t& renderer, const rect_t& viewport) {
	kontext.slice<sprite_t, location_t>().each([&renderer, &viewport](entt::entity, const sprite_t& sprite, const location_t& location) {
		if (sprite.file and sprite.layer != layer_value::Invisible) {
			if ((sprite.angle + sprite.shake) != 0.0f) {
				sprite.file->render(
					renderer,
					viewport,
					sprite.state,
					sprite.frame,
					sprite.variation,
					sprite.mirroring,
					sprite.layer,
					sprite.alpha,
					location.position,
					sprite.scale,
					sprite.angle + sprite.shake,
					sprite.pivot
				);
			} else {
				sprite.file->render(
					renderer,
					viewport,
					sprite.state,
					sprite.frame,
					sprite.variation,
					sprite.mirroring,
					sprite.layer,
					sprite.alpha,
					location.position,
					sprite.scale
				);
			}
		}
	});
}
