#ifndef SYNAO_SYSTEM_INPUT_HPP
#define SYNAO_SYSTEM_INPUT_HPP

#include <bitset>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_joystick.h>
#include <SDL2/SDL_events.h>

#include "../utility/enums.hpp"

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
	void flush();
	bool get_button_pressed(btn_t btn) const;
	bool get_button_held(btn_t btn) const;
	glm::vec2 get_position() const;
	bool has_joystick_connection() const;
	bool has_valid_recording() const;
	std::string get_scancode_name(arch_t index) const;
	std::string get_joystick_button(arch_t index) const;
	std::string get_config_name(arch_t index, bool_t is_joystick) const;
	sint_t receive_record();
	void set_nothings_recording();
	void set_keyboard_recording();
	btn_t set_keyboard_binding(sint_t code, arch_t btn);
	void set_joystick_recording();
	btn_t set_joystick_binding(sint_t code, arch_t btn);
private:
	void all_key_bindings(const setup_file_t& config);
	void all_joy_bindings(const setup_file_t& config);
public:
	std::bitset<btn_t::Total> pressed, holding;
#ifdef SYNAO_DEBUG_BUILD
	std::unordered_map<SDL_Scancode, bool_t> debug_scan;
#endif // SYNAO_DEBUG_BUILD
private:
	std::unordered_map<sint_t, btn_t> key_bind, joy_bind;
	sint_t recorder;
	glm::vec2 position;
	SDL_Joystick* joystick;
};

#endif // SYNAO_SYSTEM_INPUT_HPP