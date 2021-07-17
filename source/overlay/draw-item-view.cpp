#include "./draw-item-view.hpp"

#include "../system/kernel.hpp"

namespace {
	const glm::vec2 kViewPosition { 440.0f, 2.0f };
	const glm::vec2 kSchemePosition = kViewPosition + 3.0f;
	const glm::vec2 kAmmoPosition { 470.0f, 25.0f };
	const rect_t kAmmoBounding { 56.0f, 18.0f, 8.0f, 10.0f };
}

void draw_item_view_t::init(const texture_t* texture, const animation_t* heads_animation, const animation_t* items_animation) {
	view.set_file(heads_animation);
	view.set_state(2);
	view.set_position(kViewPosition); // 280.0f, 2.0f
	scheme.set_file(items_animation);
	scheme.set_state(0);
	scheme.set_position(kSchemePosition); // 283.0f, 5.0f
	ammo.set_texture(texture);
	ammo.set_bounding(kAmmoBounding);
	ammo.set_position(kAmmoPosition);
	ammo.set_visible(true);
	ammo.set_backwards(true);
}

void draw_item_view_t::handle(const kernel_t& kernel) {
	const glm::ivec4* item_ptr = kernel.get_item_ptr();
	if (item_ptr) {
		scheme.set_direction(
			static_cast<direction_t>(item_ptr->x - 1) // TODO: Static anaylsis doesn't like this
		);
		if ((item_ptr->y > 1 and !item_ptr->w) or (item_ptr->w != 0)) {
			ammo.set_value(item_ptr->y);
			ammo.set_visible(true);
		} else {
			ammo.set_visible(false);
		}
	} else {
		scheme.set_direction(direction_t::Invalid);
	}
}

void draw_item_view_t::render(renderer_t& renderer) const {
	if (scheme.get_direction() != direction_t::Invalid) {
		view.render(renderer);
		scheme.render(renderer);
		ammo.render(renderer);
	}
}

void draw_item_view_t::invalidate() const {
	view.invalidate();
	scheme.invalidate();
	ammo.invalidate();
}
