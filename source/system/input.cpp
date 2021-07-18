#include "./input.hpp"

#include "../resource/config.hpp"
#include "../resource/vfs.hpp"
#include "../utility/logger.hpp"
#include "../utility/rng.hpp"

#include <functional>
#include <glm/gtc/constants.hpp>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_joystick.h>

namespace {
	constexpr sint_t kScancodeNothing  = -1;
	constexpr sint_t kScancodeKeyboard = -2;
	constexpr sint_t kScancodeJoystick = -3;
}

#ifdef LEVIATHAN_USES_META
	static std::map<sint_t, bool_t> meta_pressed;
	static std::map<sint_t, bool_t> meta_holding;
#endif

input_t::~input_t() {
#ifdef LEVIATHAN_USES_META
	meta_pressed.clear();
	meta_holding.clear();
#endif
	if (device) {
		SDL_JoystickClose(device);
		device = nullptr;
	}
}

bool input_t::init(const config_t& config) {
	this->all_keyboard_bindings(config);
	this->all_joystick_bindings(config);
	this->all_macrofile_settings(config);
	scanner = kScancodeNothing;
	if (device) {
		synao_log("Error! Joystick already exists!\n");
		return false;
	}
	if (SDL_NumJoysticks() != 0) {
		device = SDL_JoystickOpen(0);
		if (!device) {
			synao_log("Joystick cannot be created at startup! SDL Error: {}\n", SDL_GetError());
			return false;
		}
	}
	synao_log("Input system initialized.\n");
	return true;
}

bool input_t::save(const config_t& config) {
	if (player and player->recording()) {
		const std::string macro = config.get_macro_file();
		if (!player->write(macro)) {
			return false;
		}
		player.reset();
	}
	return true;
}

policy_t input_t::poll(policy_t policy, bool(*callback)(const SDL_Event*)) {
	SDL_Event evt;
	while (SDL_PollEvent(&evt) != 0) {
		if (callback) {
			std::invoke(callback, &evt);
		}
		switch (evt.type) {
		case SDL_QUIT: {
			policy = policy_t::Quit;
			break;
		}
		case SDL_WINDOWEVENT: {
			if (evt.window.type == SDL_WINDOWEVENT_FOCUS_GAINED) {
				if (policy == policy_t::Stop) {
					policy = policy_t::Run;
				}
			} else if (evt.window.type == SDL_WINDOWEVENT_FOCUS_LOST) {
				if (policy == policy_t::Run) {
					policy = policy_t::Stop;
					pressed.reset();
					holding.reset();
#ifdef LEVIATHAN_USES_META
					meta_pressed.clear();
					meta_holding.clear();
#endif
				}
			}
			break;
		}
		case SDL_KEYDOWN: {
			SDL_Scancode code = evt.key.keysym.scancode;
			auto it = keyboard.find(code);
			if (it != keyboard.end()) {
				btn_t btn = it->second;
				pressed[btn] = !holding[btn];
				holding[btn] = true;
			}
			if (scanner == kScancodeKeyboard) {
				scanner = code;
			}
#ifdef LEVIATHAN_USES_META
			meta_pressed[code] = !meta_holding[code];
			meta_holding[code] = true;
#endif
			break;
		}
		case SDL_KEYUP: {
			SDL_Scancode code = evt.key.keysym.scancode;
			auto it = keyboard.find(code);
			if (it != keyboard.end()) {
				btn_t btn = it->second;
				holding[btn] = false;
			}
#ifdef LEVIATHAN_USES_META
			meta_holding[code] = false;
#endif
			break;
		}
		case SDL_MOUSEBUTTONDOWN: {
			pressed[btn_t::Click] = !holding[btn_t::Click];
			holding[btn_t::Click] = true;
#ifdef LEVIATHAN_USES_META
			meta_pressed[btn_t::Click] = !meta_holding[btn_t::Click];
			meta_holding[btn_t::Click] = true;
#endif
			break;
		}
		case SDL_MOUSEBUTTONUP: {
			holding[btn_t::Click] = false;
#ifdef LEVIATHAN_USES_META
			meta_holding[btn_t::Click] = false;
#endif
			break;
		}
		case SDL_JOYAXISMOTION: {
			static constexpr sint16_t kDeadZone = 16383;
			static constexpr uint8_t  kMoveAxis = 0;
			static constexpr uint8_t  kLookAxis = 1;
			if (evt.jaxis.which == 0) {
				uint8_t index = evt.jaxis.axis;
				sint16_t value = evt.jaxis.value;
				if (index == kMoveAxis) {
					if (value > kDeadZone) {
						pressed[btn_t::Right] = !holding[btn_t::Right];
						holding[btn_t::Right] = true;
						holding[btn_t::Left] = false;
					} else if (value < -kDeadZone) {
						pressed[btn_t::Left] = !holding[btn_t::Left];
						holding[btn_t::Left] = true;
						holding[btn_t::Right] = false;
					} else {
						holding[btn_t::Right] = false;
						holding[btn_t::Left] = false;
					}
				} else if (index == kLookAxis) {
					if (value > kDeadZone) {
						pressed[btn_t::Down] = !holding[btn_t::Down];
						holding[btn_t::Down] = true;
						holding[btn_t::Up] = false;
					} else if (value < -kDeadZone) {
						pressed[btn_t::Up] = !holding[btn_t::Up];
						holding[btn_t::Up] = true;
						holding[btn_t::Down] = false;
					} else {
						holding[btn_t::Down] = false;
						holding[btn_t::Up] = false;
					}
				}
			}
			break;
		}
		case SDL_JOYHATMOTION: {
			if (evt.jhat.which == 0) {
				uint8_t value = evt.jhat.value;
				if (value & SDL_HAT_UP) {
					pressed[btn_t::Up] = !holding[btn_t::Up];
					holding[btn_t::Up] = true;
					holding[btn_t::Down] = false;
				} else if (value & SDL_HAT_DOWN) {
					pressed[btn_t::Down] = !holding[btn_t::Down];
					holding[btn_t::Down] = true;
					holding[btn_t::Up] = false;
				} else if (value & SDL_HAT_RIGHT) {
					pressed[btn_t::Right] = !holding[btn_t::Right];
					holding[btn_t::Right] = true;
					holding[btn_t::Left] = false;
				} else if (value & SDL_HAT_LEFT) {
					pressed[btn_t::Left] = !holding[btn_t::Left];
					holding[btn_t::Left] = true;
					holding[btn_t::Right] = false;
				} else {
					holding[btn_t::Up] = false;
					holding[btn_t::Down] = false;
					holding[btn_t::Left] = false;
					holding[btn_t::Right] = false;
				}
			}
			break;
		}
		case SDL_JOYBUTTONDOWN: {
			if (evt.jbutton.which == 0) {
				sint_t code = static_cast<sint_t>(evt.jbutton.button);
				auto it = joystick.find(code);
				if (it != joystick.end()) {
					btn_t btn = it->second;
					pressed[btn] = !holding[btn];
					holding[btn] = true;
				}
			}
			break;
		}
		case SDL_JOYBUTTONUP: {
			if (evt.jbutton.which == 0) {
				sint_t code = static_cast<sint_t>(evt.jbutton.button);
				if (scanner == kScancodeJoystick) {
					scanner = code;
				}
				auto it = joystick.find(code);
				if (it != joystick.end()) {
					btn_t btn = it->second;
					holding[btn] = false;
				}
			}
			break;
		}
		case SDL_JOYDEVICEADDED: {
			if (evt.jdevice.which == 0 and !device) {
				device = SDL_JoystickOpen(0);
				if (!device) {
					synao_log("Couldn't open joystick! SDL Error: {}\n", SDL_GetError());
				}
			}
			break;
		}
		case SDL_JOYDEVICEREMOVED: {
			if (evt.jdevice.which == 0) {
				if (device) {
					SDL_JoystickClose(device);
					device = nullptr;
				}
			}
			break;
		}
		default: {
			break;
		}
		}
	}
	glm::ivec2 integral {};
	SDL_GetMouseState(&integral.x, &integral.y);
	position = glm::vec2(integral);
	return policy;
}

policy_t input_t::poll(policy_t policy) {
	return this->poll(policy, nullptr);
}

void input_t::advance() {
	if (player) {
		if (player->recording()) {
			player->store(pressed, holding);
		} else if (player->playing()) {
			player->read(pressed, holding);
		} else {
			synao_log("Macro has completed!\n");
			player.reset();
		}
	}
}

void input_t::flush() {
	pressed.reset();
#ifdef LEVIATHAN_USES_META
	meta_pressed.clear();
#endif
}

bool input_t::has_controller() const {
	return device;
}

bool input_t::has_valid_scanner() const {
	return scanner >= 0;
}

std::string input_t::get_scancode_name(arch_t index) const {
	for (auto&& pair : keyboard) {
		if (pair.second == index) {
			SDL_Scancode code = static_cast<SDL_Scancode>(pair.first);
			return SDL_GetScancodeName(code);
		}
	}
	return {};
}

std::string input_t::get_joystick_button(arch_t index) const {
	for (auto&& pair : joystick) {
		if (pair.second == index) {
			return std::to_string(pair.first);
		}
	}
	return {};
}

std::string input_t::get_config_name(arch_t index, bool_t joy) const {
	if (joy) {
		switch (index) {
			case 0: return "JoyJump";
			case 1: return "JoyHammer";
			case 2: return "JoyItem";
			case 3: return "JoyLiteDash";
			case 4: return "JoyContext";
			case 5: return "JoyStrafe";
			case 6: return "JoyInventory";
			case 7: return "JoyOptions";
		}
	}
	switch (index) {
		case 0: return "KeyJump";
		case 1: return "KeyHammer";
		case 2: return "KeyItem";
		case 3: return "KeyLiteDash";
		case 4: return "KeyContext";
		case 5: return "KeyStrafe";
		case 6: return "KeyInventory";
		case 7: return "KeyOptions";
		case 8: return "KeyUp";
		case 9: return "KeyDown";
		case 10: return "KeyLeft";
		case 11: return "KeyRight";
	}
	return "Invalid";
}

sint_t input_t::receive_scanner() {
	sint_t value = scanner;
	scanner = kScancodeNothing;
	return value;
}

void input_t::set_nothing_scanner() {
	scanner = kScancodeNothing;
}

void input_t::set_keyboard_scanner() {
	scanner = kScancodeKeyboard;
}

btn_t input_t::set_keyboard_binding(sint_t code, arch_t btn) {
	if (btn > btn_t::Right) {
		btn = btn_t::Right;
	}
	sint_t found = -1;
	for (auto&& pair : keyboard) {
		if (pair.second == btn) {
			found = pair.first;
			break;
		}
	}
	if (found != -1) {
		if (keyboard.find(code) != keyboard.end()) {
			btn_t index = keyboard[code];
			keyboard[found] = keyboard[code];
			keyboard[code] = static_cast<btn_t>(btn);
			return index;
		}
		keyboard.erase(found);
		keyboard[code] = static_cast<btn_t>(btn);
	}
	return btn_t::Total;
}

void input_t::set_joystick_scanner() {
	scanner = kScancodeJoystick;
}

btn_t input_t::set_joystick_binding(sint_t code, arch_t btn) {
	if (btn > btn_t::Options) {
		btn = btn_t::Options;
	}
	sint_t found = -1;
	for (auto&& pair : joystick) {
		if (pair.second == btn) {
			found = pair.first;
			break;
		}
	}
	if (found != -1) {
		if (joystick.find(code) != joystick.end()) {
			btn_t index = keyboard[code];
			joystick[found] = joystick[code];
			joystick[code] = static_cast<btn_t>(btn);
			return index;
		}
		joystick.erase(found);
		joystick[code] = static_cast<btn_t>(btn);
	}
	return btn_t::Total;
}

#ifdef LEVIATHAN_USES_META

bool input_t::get_meta_pressed(sint_t scancode) const {
	return meta_pressed[scancode];
}

bool input_t::get_meta_holding(sint_t scancode) const {
	return meta_holding[scancode];
}

#endif

void input_t::all_keyboard_bindings(const config_t& config) {
	keyboard[config.get_keyboard_binding(btn_t::Jump)] 		= btn_t::Jump;
	keyboard[config.get_keyboard_binding(btn_t::Hammer)] 	= btn_t::Hammer;
	keyboard[config.get_keyboard_binding(btn_t::Item)] 		= btn_t::Item;
	keyboard[config.get_keyboard_binding(btn_t::Dash)] 		= btn_t::Dash;
	keyboard[config.get_keyboard_binding(btn_t::Context)]	= btn_t::Context;
	keyboard[config.get_keyboard_binding(btn_t::Strafe)] 	= btn_t::Strafe;
	keyboard[config.get_keyboard_binding(btn_t::Inventory)] = btn_t::Inventory;
	keyboard[config.get_keyboard_binding(btn_t::Options)]	= btn_t::Options;
	keyboard[config.get_keyboard_binding(btn_t::Up)] 		= btn_t::Up;
	keyboard[config.get_keyboard_binding(btn_t::Down)] 		= btn_t::Down;
	keyboard[config.get_keyboard_binding(btn_t::Left)] 		= btn_t::Left;
	keyboard[config.get_keyboard_binding(btn_t::Right)] 	= btn_t::Right;
}

void input_t::all_joystick_bindings(const config_t& config) {
	joystick[config.get_joystick_binding(btn_t::Jump)] 		= btn_t::Jump;
	joystick[config.get_joystick_binding(btn_t::Hammer)] 	= btn_t::Hammer;
	joystick[config.get_joystick_binding(btn_t::Item)] 		= btn_t::Item;
	joystick[config.get_joystick_binding(btn_t::Dash)] 		= btn_t::Dash;
	joystick[config.get_joystick_binding(btn_t::Context)] 	= btn_t::Context;
	joystick[config.get_joystick_binding(btn_t::Strafe)] 	= btn_t::Strafe;
	joystick[config.get_joystick_binding(btn_t::Inventory)] = btn_t::Inventory;
	joystick[config.get_joystick_binding(btn_t::Options)] 	= btn_t::Options;
}

void input_t::all_macrofile_settings(const config_t& config) {
	const std::string macro = config.get_macro_file();
	bool playback = config.get_playback();
	if (!macro.empty()) {
		player = std::make_unique<macro_player_t>(!playback);
		if (!playback) {
			synao_log("Recording inputs into macro: \"{}\"...\n", macro);
		} else if (player->load(macro)) {
			synao_log("Playing inputs from macro: \"{}\"...\n", macro);
		} else {
			player.reset();
		}
	}
}

bool macro_player_t::load(const std::string& name) {
	std::vector<uint16_t> buffer {};
	sint64_t seed = 0;
	if (!vfs_t::record_buffer(vfs_t::resource_path(vfs_resource_path_t::Init) + name + ".macro", buffer, seed)) {
		synao_log("Error! Failed to load macro file!\n");
		return false;
	}
	buttons.resize(buffer.size());
	for (arch_t it = 0; it < buttons.size(); ++it) {
		buttons[it] = std::bitset<btn_t::Total>(static_cast<arch_t>(buffer[it]));
	}
	rng::seed(seed);
	index = 0;
	return true;
}

bool macro_player_t::write(const std::string& name) {
	if (!record) {
		return true;
	}
	const std::string path = vfs_t::resource_path(vfs_resource_path_t::Init) + name + ".macro";
	std::vector<uint16_t> buffer;
	buffer.resize(buttons.size());
	for (arch_t it = 0; it < buttons.size(); ++it) {
		buffer[it] = static_cast<uint16_t>(buttons[it].to_ulong());
	}
	sint64_t seed = rng::seed();
	if (!vfs_t::create_recording(path, buffer, seed)) {
		synao_log("Error! Failed to save macro file!\n");
		return false;
	}
	buttons.clear();
	record = false;
	return true;
}

void macro_player_t::read(std::bitset<btn_t::Total>& pressed, std::bitset<btn_t::Total>& holding) {
	if (this->playing()) {
		pressed = buttons[index++];
		holding = buttons[index++];
	}
}

void macro_player_t::store(const std::bitset<btn_t::Total>& pressed, const std::bitset<btn_t::Total>& holding) {
	if (this->recording()) {
		buttons.push_back(pressed);
		buttons.push_back(holding);
	}
}

bool macro_player_t::recording() const {
	return record;
}

bool macro_player_t::playing() const {
	if (record) {
		return false;
	}
	if (index == kNotReady) {
		return false;
	}
	if ((index + 1) >= buttons.size()) {
		return false;
	}
	return true;
}
