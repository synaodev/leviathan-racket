#ifndef SYNAO_UTILITY_LOGGER_HPP
#define SYNAO_UTILITY_LOGGER_HPP

#include "../types.hpp"

#ifdef SYNAO_DEBUG_BUILD

#include <cstdio>
#include <string>

namespace priv {
	template<typename ...Args>
	inline void logger(Args&& ...args) {
		sint_t r = std::printf(std::forward<Args>(args)...);
		assert(r >= 0);
	}
}
#define SYNAO_LOG(...) priv::logger(__VA_ARGS__)
#else // SYNAO_DEBUG_BUILD
#define SYNAO_LOG(...) 
#endif // SYNAO_DEBUG_BUILD

#endif // SYNAO_UTILITY_LOGGER_HPP