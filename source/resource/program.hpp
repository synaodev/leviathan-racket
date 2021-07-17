#pragma once

#include <string>

#include "../types.hpp"

namespace __enum_program {
	enum type : arch_t {
		Colors,  // Blank + Colors
		Sprites, // Major + Sprites
		Strings, // Fonts + Channels
		Total
	};
}

using program_t = __enum_program::type;

namespace program {
	std::string directive();
	std::string minor_vert();
	std::string blank_vert();
	std::string major_vert();
	std::string fonts_vert();
	std::string colors_frag();
	std::string sprites_frag();
	std::string channels_frag();
}
