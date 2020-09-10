#ifndef LEVIATHAN_INCLUDED_RESOURCE_PROGRAM_HPP
#define LEVIATHAN_INCLUDED_RESOURCE_PROGRAM_HPP

#include <string>

#include "../types.hpp"

namespace __enum_program {
	enum type : arch_t {
		Colors,  // Blank + Colors
		Sprites, // Major + Sprites
		Indexed, // Major + Indexed
		Total
	};
}

using program_t = __enum_program::type;

namespace program {
	std::string directive(glm::ivec2 version);
	std::string blank_vert(glm::ivec2 version);
	std::string major_vert(glm::ivec2 version);
	std::string colors_frag(glm::ivec2 version);
	std::string sprites_frag(glm::ivec2 version);
	std::string indexed_frag(glm::ivec2 version);
}

#endif // LEVIATHAN_INCLUDED_RESOURCE_PROGRAM_HPP
