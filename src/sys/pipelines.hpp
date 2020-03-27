#ifndef SYNAO_DISPLAY_PIPELINES_HPP
#define SYNAO_DISPLAY_PIPELINES_HPP

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

namespace pipelines {
	std::string minor_vert(glm::ivec2 version);
	std::string blank_vert(glm::ivec2 version);
	std::string major_vert(glm::ivec2 version);
	std::string colors_frag(glm::ivec2 version);
	std::string sprites_frag(glm::ivec2 version);
	std::string indexed_frag(glm::ivec2 version);
	std::string lighting_frag(glm::ivec2 version);
}

#endif // SYNAO_DISPLAY_PIPELINES_HPP