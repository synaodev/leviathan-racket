#ifndef LEVIATHAN_INCLUDED_SYSTEM_INPUT_HPP
#define LEVIATHAN_INCLUDED_SYSTEM_INPUT_HPP

#include <map>
#include <memory>
#include <SDL2/SDL_joystick.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_scancode.h>

#include "./button.hpp"

struct setup_file_t;

namespace __enum_policy {
	enum type : arch_t {
		Run,
		Stop,
		Quit
	};
}

using policy_t = __enum_policy::type;

struct input_t : public not_copyable_t {
public:
	input_t();
	input_t(input_t&&) = default;
	input_t& operator=(input_t&&) = default;
	~input_t();
public:
	bool init(const setup_file_t& config);
	bool save(const setup_file_t& config);
	policy_t poll(policy_t policy, bool(*callback)(const SDL_Event*));
	policy_t poll(policy_t policy);
	void advance();
	void flush();
	bool has_controller() const;
	bool has_valid_scanner() const;
	std::string get_scancode_name(arch_t index) const;
	std::string get_joystick_button(arch_t index) const;
	std::string get_config_name(arch_t index, bool_t joy) const;
	sint_t receive_scanner();
	void set_nothing_scanner();
	void set_keyboard_scanner();
	btn_t set_keyboard_binding(sint_t code, arch_t btn);
	void set_joystick_scanner();
	btn_t set_joystick_binding(sint_t code, arch_t btn);
#ifdef LEVIATHAN_USES_META
	static bool get_meta_pressed(SDL_Scancode scancode);
	static bool get_meta_holding(SDL_Scancode scancode);
#endif
private:
	void all_keyboard_bindings(const setup_file_t& config);
	void all_joystick_bindings(const setup_file_t& config);
	void all_demofile_settings(const setup_file_t& config);
public:
	std::bitset<btn_t::Total> pressed, holding;
	glm::vec2 position;
private:
	std::map<sint_t, btn_t> keyboard, joystick;
	std::unique_ptr<demo_player_t> player;
	sint_t scanner;
	SDL_Joystick* device;
};

#endif // LEVIATHAN_INCLUDED_SYSTEM_INPUT_HPP
