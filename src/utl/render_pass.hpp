#ifndef SYNAO_UTILITY_RENDER_PASS_HPP
#define SYNAO_UTILITY_RENDER_PASS_HPP

#include "../types.hpp"

namespace __enum_render_pass {
	enum type : arch_t {
		VtxBlankColors,
		VtxMajorSprites,
		VtxMajorIndexed,
		Total
	};
}

using render_pass_t = __enum_render_pass::type;

struct shader_t;
struct program_t;

namespace render_pass_fn {
	program_t generate(render_pass_t pass);
}

#endif // SYNAO_UTILITY_RENDER_PASS_HPP