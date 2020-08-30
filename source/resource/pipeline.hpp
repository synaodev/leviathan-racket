#ifndef LEVIATHAN_INCLUDED_RESOURCE_PIPELINE_HPP
#define LEVIATHAN_INCLUDED_RESOURCE_PIPELINE_HPP

#include <string>

#include "../types.hpp"

namespace __enum_pipeline {
	enum type : arch_t {
		VtxBlankColors,
		VtxMajorSprites,
		VtxMajorIndexed,
		Total
	};
}

using pipeline_t = __enum_pipeline::type;

namespace pipeline {
	std::string blank_vert(glm::ivec2 version);
	std::string major_vert(glm::ivec2 version);
	std::string colors_frag(glm::ivec2 version);
	std::string sprites_frag(glm::ivec2 version);
	std::string indexed_frag(glm::ivec2 version);
}

#endif // LEVIATHAN_INCLUDED_RESOURCE_PIPELINE_HPP
