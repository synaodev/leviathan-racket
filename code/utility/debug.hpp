#ifndef SYNAO_UTILITY_DEBUG_HPP
#define SYNAO_UTILITY_DEBUG_HPP

#include "../types.hpp"

namespace debug {
#ifdef SYNAO_DEBUG_BUILD
	extern bool_t Framerate;
	extern bool_t Hitboxes;
#endif // SYNAO_DEBUG_BUILD
}

#endif // SYNAO_UTILITY_DEBUG_HPP