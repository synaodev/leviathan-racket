#ifndef LEVIATHAN_INCLUDED_RESOURCE_PROGRAM_HPP
#define LEVIATHAN_INCLUDED_RESOURCE_PROGRAM_HPP

#include <string>

#include "../types.hpp"

namespace __enum_program {
	enum type : arch_t {
		Colors,  // Blank + Colors
		Sprites, // Major + Sprites
		Indexed, // Major + Indexed
		Strings, // Fonts - Channels
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
	std::string indexed_frag();
	std::string channels_frag();
}

#endif // LEVIATHAN_INCLUDED_RESOURCE_PROGRAM_HPP
