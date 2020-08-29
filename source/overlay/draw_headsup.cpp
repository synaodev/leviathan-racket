#include "./draw_headsup.hpp"

#include "../resource/id.hpp"
#include "../system/kernel.hpp"
#include "../system/receiver.hpp"
#include "../utility/meta.hpp"
#include "../utility/vfs.hpp"
#include "../utility/logger.hpp"

#include <add_on/scriptarray.h>

#ifdef LEVIATHAN_USES_META
	draw_headsup_t::draw_headsup_t() :
		suspender(),
		main_scheme(),
		leviathan_count(),
		barrier_units(),
		oxygen_count(),
		item_view(),
		fight_meter(),
		fade() {}
#else
	draw_headsup_t::draw_headsup_t() :
		suspender(),
		main_scheme(),
		leviathan_count(),
		barrier_units(),
		oxygen_count(),
		item_view(),
		fight_meter(),
		fade()
		hidden() {}
#endif

bool draw_headsup_t::init(receiver_t& receiver) {
	suspender = [&receiver] {
		receiver.suspend();
	};
	const animation_t* heads_animation = vfs::animation(res::anim::Heads);
	const animation_t* items_animation = vfs::animation(res::anim::Items);
	const texture_t* texture = vfs::texture(res::img::Heads);
	const palette_t* palette = vfs::palette(res::pal::Heads);
	if (heads_animation == nullptr or items_animation == nullptr or texture == nullptr or palette == nullptr) {
		synao_log("HeadsUp overlay is missing resources and so child overlays cannot be renderered!\n");
		return false;
	}
	main_scheme.set_file(heads_animation);
	main_scheme.set_index(0.0f);
	main_scheme.set_position(2.0f, 2.0f);

	leviathan_count.set_texture(texture);
	leviathan_count.set_palette(palette);
	leviathan_count.set_bounding(56.0f, 0.0f, 8.0f, 9.0f);
	leviathan_count.set_position(10.0f, 5.0f);
	leviathan_count.set_minimum_zeroes(3);
	leviathan_count.set_visible(true);
	leviathan_count.set_backwards(false);

	barrier_units.set_texture(texture);
	barrier_units.set_palette(palette);
	barrier_units.set_bounding(45.0f, 0.0f, 6.0f, 8.0f);
	barrier_units.set_position(47.0f, 2.0f);

	oxygen_count.set_texture(texture);
	oxygen_count.set_palette(palette);
	oxygen_count.set_bounding(56.0f, 9.0f, 8.0f, 9.0f);
	oxygen_count.set_position(2.0f, 18.0f);
	oxygen_count.set_visible(false);
	oxygen_count.set_backwards(false);

	item_view.init(texture, palette, heads_animation, items_animation);
	fight_meter.init(heads_animation);
	fade.init();
#ifdef LEVIATHAN_USES_META
	const font_t* font = vfs::debug_font();
	if (font == nullptr) {
		synao_log("Could not load debug font!\n");
		return false;
	}
	hidden.init(texture, palette, font);
#endif
	synao_log("HeadsUp overlay is ready.\n");
	return true;
}

void draw_headsup_t::reset() {
	main_scheme.set_state(0);
	main_scheme.set_direction(direction_t::Right);
	fight_meter.reset();
	fade.reset();
}

void draw_headsup_t::handle(const kernel_t& kernel) {
	item_view.handle(kernel);
	fade.handle();
}

void draw_headsup_t::update(real64_t delta) {
	main_scheme.update(delta);
	fight_meter.update(delta);
#ifdef LEVIATHAN_USES_META
	hidden.update(delta);
#endif
}

void draw_headsup_t::render(renderer_t& renderer, const kernel_t& kernel) const {
	if (!kernel.has(kernel_state_t::Lock)) {
		main_scheme.render(renderer);
		leviathan_count.render(renderer);
		barrier_units.render(renderer);
		oxygen_count.render(renderer);
		item_view.render(renderer);
		fight_meter.render(renderer);
	} else {
		main_scheme.invalidate();
		leviathan_count.invalidate();
		barrier_units.invalidate();
		oxygen_count.invalidate();
		item_view.invalidate();
		fight_meter.invalidate();
#ifdef LEVIATHAN_USES_META
		hidden.invalidate();
#endif
	}
	if (fade.is_visible()) {
		fade.render(renderer);
	}
#ifdef LEVIATHAN_USES_META
	hidden.render(renderer);
#endif
}

void draw_headsup_t::invalidate() const {
	fade.invalidate();
#ifdef LEVIATHAN_USES_META
	hidden.invalidate();
#endif
}

void draw_headsup_t::set_parameters(headsup_params_t params) {
	main_scheme.set_index(params.main_state);
	main_scheme.set_direction(params.main_direction);
	leviathan_count.set_value(params.current_leviathan);
	barrier_units.set_values(params.current_barrier, params.maximum_barrier);
	oxygen_count.set_visible(params.current_oxygen != params.maximum_oxygen);
	oxygen_count.set_value(params.current_oxygen);
}

void draw_headsup_t::set_fight_values(sint_t current, sint_t maximum) {
	fight_meter.set_values(current, maximum);
}

#ifdef LEVIATHAN_USES_META

void draw_headsup_t::set_hidden_state(draw_hidden_state_t state, std::function<sint_t()> radio) {
	hidden.set_state(state, radio);
}

#endif

void draw_headsup_t::fade_in() {
	fade.fade_in();
	std::invoke(suspender);
}

void draw_headsup_t::fade_out() {
	fade.fade_out();
	std::invoke(suspender);
}

bool draw_headsup_t::is_fade_done() const {
	return fade.is_done();
}

bool draw_headsup_t::is_fade_moving() const {
	return fade.is_moving();
}

real_t draw_headsup_t::get_main_index() const {
	return main_scheme.get_index();
}
