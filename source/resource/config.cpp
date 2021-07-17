#include "./config.hpp"

#include <fstream>

#include "../utility/logger.hpp"

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
	ofs.write(output.c_str(), output.size());
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
