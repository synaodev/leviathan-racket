#include "./draw_item_view.hpp"

#include "../system/kernel.hpp"

draw_item_view_t::draw_item_view_t() :
	rect(),
	scheme(),
	ammo()
{

}

void draw_item_view_t::init(const texture_t* texture, const palette_t* palette, const animation_t* heads_animation, const animation_t* items_animation) {
	rect.set_file(heads_animation);
	rect.set_state(2);
	rect.set_position(280.0f, 2.0f);
	scheme.set_file(items_animation);
	scheme.set_state(0);
	scheme.set_position(283.0f, 5.0f);
	ammo.set_texture(texture);
	ammo.set_palette(palette);
	ammo.set_bounding(56.0f, 18.0f, 8.0f, 10.0f);
	ammo.set_position(310.0f, 25.0f);
	ammo.set_visible(true);
	ammo.set_backwards(true);
}

void draw_item_view_t::handle(const kernel_t& kernel) {
	const glm::ivec4* item_ptr = kernel.get_item_ptr();
	if (item_ptr != nullptr) {
		scheme.set_direction(
			static_cast<direction_t>(item_ptr->x - 1)
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
		rect.render(renderer);
		scheme.render(renderer);
		ammo.render(renderer);
	}
}

void draw_item_view_t::force() const {
	rect.force();
	scheme.force();
	ammo.force();
}
