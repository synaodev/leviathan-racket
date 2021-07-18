#pragma once

#include <memory>
#include <bitset>
#include <vector>
#include <string>
#include <map>
#include <glm/vec2.hpp>

#include "../utility/enums.hpp"

struct config_t;
struct macro_player_t;

enum class policy_t {
	Run,
	Stop,
	Quit
};

namespace __enum_btn {
	enum type : arch_t {
		Jump, Yes = Jump,
		Hammer, No = Hammer,
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
		Click,
		Total = 16
	};
}

using btn_t = __enum_btn::type;

typedef union SDL_Event SDL_Event;
typedef struct _SDL_Joystick SDL_Joystick;

struct input_t : public not_copyable_t, public not_moveable_t {
public:
	input_t() = default;
	~input_t();
public:
	bool init(const config_t& config);
	bool save(const config_t& config);
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
	bool get_meta_pressed(sint_t scancode) const;
	bool get_meta_holding(sint_t scancode) const;
#endif
private:
	void all_keyboard_bindings(const config_t& config);
	void all_joystick_bindings(const config_t& config);
	void all_macrofile_settings(const config_t& config);
public:
	std::bitset<btn_t::Total> pressed { 0 };
	std::bitset<btn_t::Total> holding { 0 };
	glm::vec2 position {};
private:
	std::map<sint_t, btn_t> keyboard {};
	std::map<sint_t, btn_t> joystick {};
	std::unique_ptr<macro_player_t> player { nullptr };
	sint_t scanner { 0 };
	SDL_Joystick* device { nullptr };
};

struct macro_player_t : public not_copyable_t {
public:
	macro_player_t() = default;
	macro_player_t(bool_t record) :
		record(record) {}
	macro_player_t(macro_player_t&&) noexcept = default;
	macro_player_t& operator=(macro_player_t&&) noexcept = default;
	~macro_player_t() = default;
public:
	bool load(const std::string& name);
	bool write(const std::string& output);
	void read(std::bitset<btn_t::Total>& pressed, std::bitset<btn_t::Total>& holding);
	void store(const std::bitset<btn_t::Total>& pressed, const std::bitset<btn_t::Total>& holding);
	bool recording() const;
	bool playing() const;
public:
	static constexpr arch_t kNotReady = (arch_t)-1;
private:
	bool_t record { false };
	arch_t index { kNotReady };
	std::vector<std::bitset<btn_t::Total> > buttons {};
};
