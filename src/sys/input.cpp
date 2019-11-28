#include "./input.hpp"

#include "../utl/logger.hpp"
#include "../utl/setup_file.hpp"

#include <SDL2/SDL_events.h>
#include <SDL2/SDL_timer.h>

input_t::input_t() :
	pressed(0),
	holding(0),
	key_bind(),
	joy_bind(),
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
		SYNAO_LOG("Joystick already exists!");
		return false;
	}
	if (SDL_NumJoysticks() != 0) {
		joystick = SDL_JoystickOpen(0);
		if (joystick == nullptr) {
			SYNAO_LOG(
				"Joystick cannot be created at startup! SDL Error: %s\n", 
				SDL_GetError()
			);
			return false;
		}
	}
	return true;
}

void input_t::poll(policy_t& policy) {
	SDL_Event evt;
	while(SDL_PollEvent(&evt)) {
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
				}
			}
			break;
		}
		case SDL_KEYDOWN: {
			SDL_Scancode code = evt.key.keysym.scancode;
			auto it = key_bind.find(code);
			if (it != key_bind.end()) {
				btn_t btn = it->second;
				pressed[btn] = !holding[btn];
				holding[btn] = true;
			}
			break;
		}
		case SDL_KEYUP: {
			SDL_Scancode code = evt.key.keysym.scancode;
			auto it = key_bind.find(code);
			if (it != key_bind.end()) {
				btn_t btn = it->second;
				holding[btn] = false;
			}
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
}

bool input_t::get_button_pressed(btn_t btn) const {
	return pressed[btn];
}

bool input_t::get_button_held(btn_t btn) const {
	return holding[btn];
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
	sint_t editor	= SDL_SCANCODE_GRAVE;

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
	key_bind[editor]	= btn_t::Editor;
}

void input_t::all_joy_bindings(const setup_file_t&) {
	joy_bind[0] = btn_t::Jump;
	joy_bind[1] = btn_t::Hammer;
	joy_bind[2] = btn_t::Item;
	joy_bind[3] = btn_t::Dash;
	joy_bind[4] = btn_t::Context;
	joy_bind[5] = btn_t::Strafe;
	joy_bind[6] = btn_t::Inventory;
	joy_bind[7] = btn_t::Options;
}