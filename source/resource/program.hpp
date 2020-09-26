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
	std::string directive(const glm::ivec2& version);
	std::string blank_vert(const glm::ivec2& version);
	std::string major_vert(const glm::ivec2& version);
	std::string fonts_vert(const glm::ivec2& version);
	std::string colors_frag(const glm::ivec2& version);
	std::string sprites_frag(const glm::ivec2& version);
	std::string indexed_frag(const glm::ivec2& version);
	std::string channels_frag(const glm::ivec2& version);
}

#endif // LEVIATHAN_INCLUDED_RESOURCE_PROGRAM_HPP
