#include "./sprite.hpp"
#include "./location.hpp"
#include "./kontext.hpp"

#include "../utl/vfs.hpp"

sprite_t::sprite_t(const std::string& name) :
	file(nullptr),
	write(false),
	timer(0.0),
	alpha(0.0f),
	table(0.0f),
	state(0),
	direction(direction_t::Right),
	frame(0),
	layer(layer_value::Automatic),
	scale(1.0f),
	position(0.0f),
	pivot(0.0f),
	angle(0.0f),
	shake(0.0f),
	power(0.0f)
{
	file = vfs::animation(name);
}

sprite_t::sprite_t() :
	file(nullptr),
	write(false),
	timer(0.0),
	alpha(0.0f),
	table(0.0f),
	state(0),
	direction(direction_t::Right),
	frame(0),
	layer(layer_value::Automatic),
	scale(1.0f),
	position(0.0f),
	pivot(0.0f),
	angle(0.0f),
	shake(0.0f),
	power(0.0f)
{

}

void sprite_t::reset() {
	write = true;
	timer = 0.0;
	alpha = 0.0f;
	table = 0.0f;
	state = 0;
	direction = direction_t::Right;
	frame = 0;
	layer = layer_value::Automatic;
	scale = glm::one<glm::vec2>();
	position = glm::zero<glm::vec2>();
	pivot = glm::zero<glm::vec2>();
	angle = 0.0f;
	shake = 0.0f;
	power = 0.0f;
}

void sprite_t::new_state(arch_t state) {
	if (state != NonState) {
		if (this->state != state) {
			this->write = true;
			this->timer = 0.0;
			this->state = state;
			this->frame = 0;
		}
	}
}

glm::vec2 sprite_t::action_point(arch_t state, direction_t direction, glm::vec2 position) const {
	if (file != nullptr) {
		return file->get_action_point(state, direction);
	}
	return glm::zero<glm::vec2>();
}

bool sprite_t::finished() const {
	if (file != nullptr) {
		return file->is_finished(state, frame, timer);
	}
	return true;
}

void sprite_t::update(kontext_t& kontext, real64_t delta) {
	kontext.slice<sprite_t, location_t>().each([delta](entt::entity actor, sprite_t& sprite, const location_t& location) {
		if (sprite.file != nullptr) {
			sprite.file->update(
				delta,
				sprite.write,
				sprite.state,
				sprite.timer,
				sprite.frame
			);
			if (sprite.position != location.position) {
				sprite.write = true;
				sprite.position = location.position;
			}
			if (sprite.power != 0.0f) {
				sprite.write = true;
				sprite.shake -= static_cast<real_t>(delta);
				sprite.power = (sprite.shake > 0.0f) ? -sprite.power : 0.0f;
			}
		}
	});
}

void sprite_t::render(const kontext_t& kontext, renderer_t& renderer, rect_t viewport, bool_t panic) {
	static arch_t previous = 0;
	arch_t current = 0;
	const entt::view<sprite_t> view = kontext.slice<sprite_t>();
	if (!panic) {
		view.each([&viewport, &current](entt::entity actor, const sprite_t& sprite) {
			if (sprite.file != nullptr) {
				if (sprite.file->visible(
					viewport,
					sprite.state,
					sprite.frame,
					sprite.direction, 
					sprite.layer,
					sprite.position,
					sprite.scale
				)) {
					++current;
				}
			}
		});
		panic = previous != current;
		previous = current;
	}
	view.each([&renderer, &viewport, panic](entt::entity actor, const sprite_t& sprite) {
		if (sprite.file != nullptr and sprite.layer != layer_value::Invisible) {
			if ((sprite.angle + sprite.shake) != 0.0f) {
				sprite.file->render(
					renderer,
					viewport,
					panic,
					sprite.write,
					sprite.state,
					sprite.frame,
					sprite.direction,
					sprite.layer,
					sprite.alpha,
					sprite.table,
					glm::round(sprite.position),
					sprite.scale,
					sprite.angle + sprite.power,
					sprite.pivot
				);
			} else {
				sprite.file->render(
					renderer, 
					viewport, 
					panic, 
					sprite.write,
					sprite.state,
					sprite.frame,
					sprite.direction,
					sprite.layer,
					sprite.alpha,
					sprite.table,
					glm::round(sprite.position),
					sprite.scale
				);
			}
		}
	});
}