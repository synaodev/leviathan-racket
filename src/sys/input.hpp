#ifndef SYNAO_MAIN_INPUT_HPP
#define SYNAO_MAIN_INPUT_HPP

#include <bitset>
#include <string>
#include <unordered_map>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_joystick.h>
#include <SDL2/SDL_events.h>

#include "../utl/enums.hpp"

struct setup_file_t;

namespace __enum_policy {
	enum type : arch_t {
		Run,
		Stop,
		Quit
	};
}

using policy_t = __enum_policy::type;

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
		ClickL,
		ClickR,
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
	policy_t poll(policy_t policy, bool(*callback)(const SDL_Event*));
	policy_t poll(policy_t policy);
	bool get_button_pressed(btn_t btn) const;
	bool get_button_held(btn_t btn) const;
	std::string get_scancode_name(arch_t index) const;
	std::string get_joystick_button(arch_t index) const;
	void set_keyboard_binding(SDL_Scancode code, btn_t btn);
	void set_joystick_binding(sint_t code, btn_t btn);
public:
	std::bitset<btn_t::Total> pressed, holding;
	glm::vec2 position;
private:
	void all_key_bindings(const setup_file_t& config);
	void all_joy_bindings(const setup_file_t& config);
private:
	std::unordered_map<sint_t, btn_t> key_bind, joy_bind;
	SDL_Joystick* joystick;
};

#endif // SYNAO_MAIN_INPUT_HPP