#include "./input.hpp"

#include "../utility/logger.hpp"
#include "../utility/setup_file.hpp"

#include <functional>

static constexpr sint_t kRecordNothings = -1;
static constexpr sint_t kRecordKeyboard = -2;
static constexpr sint_t kRecordJoystick = -3;

input_t::input_t() :
	pressed(0),
	holding(0),
#ifdef SYNAO_DEBUG_BUILD
	debug_pressed(),
	debug_holding(),
#endif // SYNAO_DEBUG_BUILD
	key_bind(),
	joy_bind(),
	recorder(kRecordNothings),
	position(0.0f),
	joystick(nullptr)
{

}

input_t::~input_t() {
	if (joystick != nullptr) {
		SDL_JoystickClose(joystick);
		joystick = nullptr;
	}
}

bool input_t::init(const setup_file_t& config) {
	this->all_key_bindings(config);
	this->all_joy_bindings(config);
	if (joystick != nullptr) {
		SYNAO_LOG("Error! Joystick already exists!\n");
		return false;
	}
	if (SDL_NumJoysticks() != 0) {
		joystick = SDL_JoystickOpen(0);
		if (joystick == nullptr) {
			SYNAO_LOG("Joystick cannot be created at startup! SDL Error: %s\n", SDL_GetError());
			return false;
		}
	}
	return true;
}

policy_t input_t::poll(policy_t policy, bool(*callback)(const SDL_Event*)) {
	SDL_Event evt;
	while (SDL_PollEvent(&evt) != 0) {
		if (callback != nullptr) {
			std::invoke(callback, &evt);
		}
		switch (evt.type) {
		case SDL_QUIT: {
			policy = policy_t::Quit;
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
#ifdef SYNAO_DEBUG_BUILD
					debug_pressed.clear();
					debug_holding.clear();
#endif // SYNAO_DEBUG_BUILD
				}
			}
			break;
		}
		case SDL_KEYDOWN: {
			SDL_Scancode code = evt.key.keysym.scancode;
#ifdef SYNAO_DEBUG_BUILD
			debug_pressed[code] = !debug_holding[code];
			debug_holding[code] = true;
#endif // SYNAO_DEBUG_BUILD
			auto it = key_bind.find(code);
			if (it != key_bind.end()) {
				btn_t btn = it->second;
				pressed[btn] = !holding[btn];
				holding[btn] = true;
			}
			if (recorder == kRecordKeyboard) {
				recorder = code;
			}
			break;
		}
		case SDL_KEYUP: {
			SDL_Scancode code = evt.key.keysym.scancode;
#ifdef SYNAO_DEBUG_BUILD
			debug_holding[code] = false;
#endif // SYNAO_DEBUG_BUILD
			auto it = key_bind.find(code);
			if (it != key_bind.end()) {
				btn_t btn = it->second;
				holding[btn] = false;
			}
			break;
		}
		case SDL_MOUSEBUTTONDOWN: {
			btn_t btn = evt.button.button == SDL_BUTTON_LEFT ? 
				btn_t::ClickL :
				btn_t::ClickR;
			pressed[btn] = !holding[btn];
			holding[btn] = true;
			break;
		}
		case SDL_MOUSEBUTTONUP: {
			btn_t btn = evt.button.button == SDL_BUTTON_LEFT ? 
				btn_t::ClickL :
				btn_t::ClickR;
			holding[btn] = false;
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
		case SDL_JOYBUTTONUP: {
			if (evt.jbutton.which == 0) {
				sint_t code = static_cast<sint_t>(evt.jbutton.button);
				auto it = joy_bind.find(code);
				if (it != joy_bind.end()) {
					btn_t btn = it->second;
					pressed[btn] = !holding[btn];
					holding[btn] = true;
				}
			}
			break;
		}
		case SDL_JOYBUTTONDOWN: {
			if (evt.jbutton.which == 0) {
				sint_t code = static_cast<sint_t>(evt.jbutton.button);
				if (recorder == kRecordJoystick) {
					recorder = code;
				}
				auto it = joy_bind.find(code);
				if (it != joy_bind.end()) {
					btn_t btn = it->second;
					holding[btn] = false;
				}
			}
			break;
		}
		case SDL_JOYDEVICEADDED: {
			if (evt.jdevice.which == 0 and joystick == nullptr) {
				joystick = SDL_JoystickOpen(0);
				if (joystick == nullptr) {
					SYNAO_LOG(
						"Couldn't open joystick! SDL Error: %s\n", 
						SDL_GetError()
					);
				}
			}
			break;
		}
		case SDL_JOYDEVICEREMOVED: {
			if (evt.jdevice.which == 0) {
				if (joystick != nullptr) {
					SDL_JoystickClose(joystick);
					joystick = nullptr;
				}
			}
			break;
		}
		default: {
			break;
		}
		}
	}
	glm::ivec2 integral_position = glm::zero<glm::ivec2>();
	SDL_GetMouseState(
		&integral_position.x, 
		&integral_position.y
	);
	position = glm::vec2(integral_position);
	return policy;
}

policy_t input_t::poll(policy_t policy) {
	return this->poll(policy, nullptr);
}

void input_t::flush() {
	pressed.reset();
#ifdef SYNAO_DEBUG_BUILD
	debug_pressed.clear();
#endif // SYNAO_DEBUG_BUILD
}

bool input_t::get_button_pressed(btn_t btn) const {
	return pressed[btn];
}

bool input_t::get_button_held(btn_t btn) const {
	return holding[btn];
}

glm::vec2 input_t::get_position() const {
	return position;
}

bool input_t::has_joystick_connection() const {
	return joystick != nullptr;
}

bool input_t::has_valid_recording() const {
	return recorder >= 0;
}

std::string input_t::get_scancode_name(arch_t index) const {
	for (auto&& pair : key_bind) {
		if (pair.second == index) {
			SDL_Scancode code = static_cast<SDL_Scancode>(pair.first);
			return SDL_GetScancodeName(code);
		}
	}
	return std::string();
}

std::string input_t::get_joystick_button(arch_t index) const {
	for (auto&& pair : joy_bind) {
		if (pair.second == index) {
			return std::to_string(pair.first);
		}
	}
	return std::string();
}

std::string input_t::get_config_name(arch_t index, bool_t is_joystick) const {
	if (is_joystick) {
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

sint_t input_t::receive_record() {
	sint_t value = recorder;
	recorder = kRecordNothings;
	return value;
}

void input_t::set_nothings_recording() {
	recorder = kRecordNothings;
}

void input_t::set_keyboard_recording() {
	recorder = kRecordKeyboard;
}

btn_t input_t::set_keyboard_binding(sint_t code, arch_t btn) {
	if (btn > btn_t::Right) {
		btn = btn_t::Right;
	}
	sint_t found = -1;
	for (auto&& pair : key_bind) {
		if (pair.second == btn) {
			found = pair.first;
			break;
		}
	}
	if (found != -1) {
		if (key_bind.find(code) != key_bind.end()) {
			btn_t index = key_bind[code];
			key_bind[found] = key_bind[code];
			key_bind[code] = static_cast<btn_t>(btn);
			return index;
		}
		key_bind.erase(found);
		key_bind[code] = static_cast<btn_t>(btn);
	}
	return btn_t::Total;
}

void input_t::set_joystick_recording() {
	recorder = kRecordJoystick;
}

btn_t input_t::set_joystick_binding(sint_t code, arch_t btn) {
	if (btn > btn_t::Options) {
		btn = btn_t::Options;
	}
	sint_t found = -1;
	for (auto&& pair : joy_bind) {
		if (pair.second == btn) {
			found = pair.first;
			break;
		}
	}
	if (found != -1) {
		if (joy_bind.find(code) != joy_bind.end()) {
			btn_t index = key_bind[code];
			joy_bind[found] = joy_bind[code];
			joy_bind[code] = static_cast<btn_t>(btn);
			return index;
		}
		joy_bind.erase(found);
		joy_bind[code] = static_cast<btn_t>(btn);
	}
	return btn_t::Total;
}

void input_t::all_key_bindings(const setup_file_t& config) {
	sint_t jump		= SDL_SCANCODE_Z;
	sint_t hammer	= SDL_SCANCODE_X;
	sint_t item		= SDL_SCANCODE_LSHIFT;
	sint_t litedash = SDL_SCANCODE_A;
	sint_t context	= SDL_SCANCODE_SPACE;
	sint_t strafe	= SDL_SCANCODE_LCTRL;
	sint_t inven	= SDL_SCANCODE_TAB;
	sint_t options	= SDL_SCANCODE_ESCAPE;
	sint_t up		= SDL_SCANCODE_UP;
	sint_t down		= SDL_SCANCODE_DOWN;
	sint_t left		= SDL_SCANCODE_LEFT;
	sint_t right	= SDL_SCANCODE_RIGHT;

	config.get("Input", "KeyJump",		jump);
	config.get("Input", "KeyHammer",	hammer);
	config.get("Input", "KeyItem",		item);
	config.get("Input", "KeyLiteDash",	litedash);
	config.get("Input", "KeyContext",	context);
	config.get("Input", "KeyStrafe",	strafe);
	config.get("Input", "KeyInventory",	inven);
	config.get("Input", "KeyOptions",	options);
	config.get("Input", "KeyUp",		up);
	config.get("Input", "KeyDown",		down);
	config.get("Input", "KeyLeft",		left);
	config.get("Input", "KeyRight",		right);

	key_bind[jump]		= btn_t::Jump;
	key_bind[hammer]	= btn_t::Hammer;
	key_bind[item]		= btn_t::Item;
	key_bind[litedash]	= btn_t::Dash;
	key_bind[context]	= btn_t::Context;
	key_bind[strafe]	= btn_t::Strafe;
	key_bind[inven]		= btn_t::Inventory;
	key_bind[options]	= btn_t::Options;
	key_bind[up]		= btn_t::Up;
	key_bind[down]		= btn_t::Down;
	key_bind[left]		= btn_t::Left;
	key_bind[right]		= btn_t::Right;
}

void input_t::all_joy_bindings(const setup_file_t& config) {
	sint_t jump		= 0;
	sint_t hammer	= 1;
	sint_t item		= 2;
	sint_t litedash = 3;
	sint_t context	= 4;
	sint_t strafe	= 5;
	sint_t inven	= 6;
	sint_t options	= 7;

	config.get("Input", "JoyJump",		jump);
	config.get("Input", "JoyHammer",	hammer);
	config.get("Input", "JoyItem",		item);
	config.get("Input", "JoyLiteDash",	litedash);
	config.get("Input", "JoyContext",	context);
	config.get("Input", "JoyStrafe",	strafe);
	config.get("Input", "JoyInventory",	inven);
	config.get("Input", "JoyOptions",	options);

	joy_bind[jump] 		= btn_t::Jump;
	joy_bind[hammer] 	= btn_t::Hammer;
	joy_bind[item] 		= btn_t::Item;
	joy_bind[litedash] 	= btn_t::Dash;
	joy_bind[context] 	= btn_t::Context;
	joy_bind[strafe] 	= btn_t::Strafe;
	joy_bind[inven] 	= btn_t::Inventory;
	joy_bind[options] 	= btn_t::Options;
}