#include "./config.hpp"

#include <fstream>
#include <SDL2/SDL_scancode.h>

#include "../utility/logger.hpp"

void config_t::generate(const std::string& path) {
	this->path = path;
	valid = true;
	data.clear();

	this->set_meta_menu(false);
	this->set_legacy_gl(false);
	this->set_language("english");

	this->set_vertical_sync(false);
	this->set_fullscreen(false);
	this->set_scaling(2);
	this->set_framerate(60.0);

	this->set_audio_volume(1.0f);

	this->set_music_volume(0.35f);
	this->set_channels(2);
	this->set_sampling_rate(44100);
	this->set_buffered_time(0.1f);

	this->set_keyboard_binding(0, SDL_SCANCODE_Z);
	this->set_keyboard_binding(1, SDL_SCANCODE_X);
	this->set_keyboard_binding(2, SDL_SCANCODE_LSHIFT);
	this->set_keyboard_binding(3, SDL_SCANCODE_A);
	this->set_keyboard_binding(4, SDL_SCANCODE_SPACE);
	this->set_keyboard_binding(5, SDL_SCANCODE_LCTRL);
	this->set_keyboard_binding(6, SDL_SCANCODE_TAB);
	this->set_keyboard_binding(7, SDL_SCANCODE_ESCAPE);
	this->set_keyboard_binding(8, SDL_SCANCODE_UP);
	this->set_keyboard_binding(9, SDL_SCANCODE_DOWN);
	this->set_keyboard_binding(10, SDL_SCANCODE_LEFT);
	this->set_keyboard_binding(11, SDL_SCANCODE_RIGHT);

	this->set_joystick_binding(0, 0);
	this->set_joystick_binding(1, 1);
	this->set_joystick_binding(2, 2);
	this->set_joystick_binding(3, 3);
	this->set_joystick_binding(4, 4);
	this->set_joystick_binding(5, 5);
	this->set_joystick_binding(6, 6);
	this->set_joystick_binding(7, 7);
}

bool config_t::load(const std::string& path) {
	valid = false;
	std::ifstream ifs { path, std::ios::binary };
	if (!ifs.is_open()) {
		synao_log("Failed to read boot config file: {}!\n", path);
		return false;
	}
	valid = true;
	data = nlohmann::json::parse(ifs);
	this->path = path;
	return true;
}

bool config_t::save() {
	if (!valid) {
		return false;
	}
	std::ofstream ofs { path, std::ios::binary };
	if (!ofs.is_open()) {
		synao_log("Failed to write boot config file: {}!\n", path);
		return false;
	}
	const std::string output = data.dump(
		1, '\t', true,
		nlohmann::detail::error_handler_t::ignore
	);
	ofs.write(output.data(), output.size());
	return true;
}

std::string config_t::get_keyboard_name(arch_t index) {
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
		default: return {};
	}
}

std::string config_t::get_joystick_name(arch_t index) {
	switch (index) {
		case 0: return "JoyJump";
		case 1: return "JoyHammer";
		case 2: return "JoyItem";
		case 3: return "JoyLiteDash";
		case 4: return "JoyContext";
		case 5: return "JoyStrafe";
		case 6: return "JoyInventory";
		case 7: return "JoyOptions";
		default: return {};
	}
}
