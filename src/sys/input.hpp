#ifndef SYNAO_MAIN_INPUT_HPP
#define SYNAO_MAIN_INPUT_HPP

#include <bitset>
#include <unordered_map>
#include <SDL2/SDL_joystick.h>

#include "../utl/enums.hpp"

struct setup_file_t;

namespace __enum_btn {
	enum type : arch_t {
		Jump, 
		Yes = Jump,
		Hammer, 
		No = Hammer,
		Item, 
		Dash,
		Context, 
		Strafe,
		Inventory, 
		Options,
		Up, 
		Down, 
		Left, 
		Right,
		Editor, 
		Total
	};
}

using btn_t = __enum_btn::type;

struct input_t : public not_copyable_t {
public:
	input_t();
	input_t(input_t&&) = default;
	input_t& operator=(input_t&&) = default;
	~input_t();
public:
	bool init(const setup_file_t& config);
	void poll(policy_t& policy);
	bool get_button_pressed(btn_t btn) const;
	bool get_button_held(btn_t btn) const;
public:
	std::bitset<btn_t::Total> pressed, holding;
private:
	void all_key_bindings(const setup_file_t& config);
	void all_joy_bindings(const setup_file_t& config);
private:
	std::unordered_map<sint_t, btn_t> key_bind, joy_bind;
	SDL_Joystick* joystick;
};

#endif // SYNAO_MAIN_INPUT_HPP