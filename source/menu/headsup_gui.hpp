#ifndef LEVIATHAN_INCLUDED_MENU_HEADSUP_GUI_HPP
#define LEVIATHAN_INCLUDED_MENU_HEADSUP_GUI_HPP

#include <functional>

#include "../overlay/draw_title_view.hpp"
#include "../overlay/draw_count.hpp"
#include "../overlay/draw_units.hpp"
#include "../overlay/draw_scheme.hpp"
#include "../overlay/draw_fade.hpp"
#include "../overlay/draw_item_view.hpp"
#include "../overlay/draw_meter.hpp"
#include "../overlay/draw_hidden.hpp"

struct input_t;
struct audio_t;
struct kernel_t;
struct receiver_t;

struct headsup_params_t {
public:
	sint_t current_barrier, maximum_barrier, current_leviathan;
	real_t main_state;
	direction_t main_direction;
	sint_t current_oxygen, maximum_oxygen;
public:
	headsup_params_t() :
		current_barrier(0),
		maximum_barrier(0),
		current_leviathan(0),
		main_state(0.0f),
		main_direction(direction_t::Right),
		current_oxygen(0),
		maximum_oxygen(0) {}
	headsup_params_t(const headsup_params_t&) = default;
	headsup_params_t(headsup_params_t&&) = default;
	headsup_params_t& operator=(const headsup_params_t&) = default;
	headsup_params_t& operator=(headsup_params_t&&) = default;
	~headsup_params_t() = default;
};

struct headsup_gui_t : public not_copyable_t {
public:
	headsup_gui_t();
	headsup_gui_t(headsup_gui_t&&) = default;
	headsup_gui_t& operator=(headsup_gui_t&&) = default;
	~headsup_gui_t() = default;
public:
	bool init(receiver_t& receiver);
	void reset();
	void handle(const kernel_t& kernel);
	void update(real64_t delta);
	void render(renderer_t& renderer, const kernel_t& kernel) const;
	void invalidate() const;
	void set_parameters(headsup_params_t params);
	void set_fight_values(sint_t current, sint_t maximum);
#ifdef LEVIATHAN_USES_META
	void set_hidden_state(draw_hidden_state_t state, std::function<sint_t()> radio);
#endif
	void fade_in();
	void fade_out();
	void set_field_text(const std::string& text);
	void set_field_text();
	void push_card(const std::string& text, arch_t font_index);
	void clear_cards();
	void set_card_position(arch_t index, real_t x, real_t y);
	void set_card_centered(arch_t index, bool horizontal, bool vertical);
	bool is_fade_done() const;
	bool is_fade_moving() const;
	real_t get_main_index() const;
private:
	std::function<void(void)> suspender;
	draw_title_view_t title_view;
	draw_scheme_t main_scheme;
	draw_count_t leviathan_count;
	draw_units_t barrier_units;
	draw_count_t oxygen_count;
	draw_item_view_t item_view;
	draw_meter_t fight_meter;
	draw_fade_t fade;
#ifdef LEVIATHAN_USES_META
	draw_hidden_t hidden;
#endif
};

#endif // LEVIATHAN_INCLUDED_MENU_HEADSUP_GUI_HPP