#include "./draw_headsup.hpp"

#include "../utl/misc.hpp"
#include "../utl/vfs.hpp"

#include "../eve/receiver.hpp"
#include "../eve/array.hpp"

#include "../sys/kernel.hpp"

#include "../res_id.hpp"

draw_headsup_t::draw_headsup_t() :
	main_scheme(),
	leviathan_count(),
	barrier_units(),
	oxygen_count(),
	item_view(),
	fade(),
	framerate(),
	suspender()
{

}

bool draw_headsup_t::init(receiver_t& receiver) {
	const animation_t* animation = vfs::animation(res::anim::Heads);
	if (!animation) {
		return false;
	}
	const texture_t* texture = vfs::texture(res::img::Heads);
	if (!texture) {
		return false;
	}
	main_scheme.set_file(animation);
	main_scheme.set_state(0);
	main_scheme.set_position(2.0f, 2.0f);

	leviathan_count.set_texture(texture);
	leviathan_count.set_bounding(156.0f, 0.0f, 8.0f, 9.0f);
	leviathan_count.set_position(10.0f, 5.0f);
	leviathan_count.set_minimum_zeroes(3);
	leviathan_count.set_visible(true);
	leviathan_count.set_backwards(false);

	barrier_units.set_texture(texture);
	barrier_units.set_bounding(145.0f, 0.0f, 6.0f, 8.0f);
	barrier_units.set_position(47.0f, 2.0f);

	oxygen_count.set_texture(texture);
	oxygen_count.set_bounding(156.0f, 9.0f, 8.0f, 9.0f);
	oxygen_count.set_position(2.0f, 18.0f);
	oxygen_count.set_visible(false);
	oxygen_count.set_backwards(false);

	item_view.init(texture, animation);
	fade.init();
	framerate.init(texture);
	suspender.connect<&receiver_t::suspend>(&receiver);
	return true;
}

void draw_headsup_t::reset() {
	main_scheme.set_state(0);
	main_scheme.set_direction(direction_t::Right);
	fade.reset();
}

void draw_headsup_t::handle(const kernel_t& kernel) {
	item_view.handle(kernel);
	fade.handle();
}

void draw_headsup_t::update(real64_t delta) {
	main_scheme.update(delta);
	if constexpr (misc::kFramerate) {
		framerate.update(delta);
	}
}

void draw_headsup_t::render(renderer_t& renderer, const kernel_t& kernel) const {
	if (!kernel.has(kernel_state_t::Lock)) {
		main_scheme.render(renderer);
		leviathan_count.render(renderer);
		barrier_units.render(renderer);
		oxygen_count.render(renderer);
		item_view.render(renderer);
	} else {
		main_scheme.force();
		leviathan_count.force();
		barrier_units.force();
		oxygen_count.force();
		item_view.force();
		if constexpr (misc::kFramerate) {
			framerate.force();
		}
	}
	if (fade.is_visible()) {
		fade.render(renderer);
	}
	if constexpr (misc::kFramerate) {
		framerate.render(renderer);
	}
}

void draw_headsup_t::set_parameters(headsup_params_t params) {
	main_scheme.set_state(params.main_state);
	main_scheme.set_direction(params.main_direction);
	leviathan_count.set_value(params.current_leviathan);
	barrier_units.set_values(params.current_barrier, params.maximum_barrier);
	oxygen_count.set_visible(params.current_barrier != params.maximum_oxygen);
	oxygen_count.set_value(params.current_oxygen);
}

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

arch_t draw_headsup_t::get_main_state() const {
	return main_scheme.get_state();
}
