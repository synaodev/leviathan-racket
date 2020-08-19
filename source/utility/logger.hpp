#ifndef LEVIATHAN_INCLUDED_UTILITY_LOGGER_HPP
#define LEVIATHAN_INCLUDED_UTILITY_LOGGER_HPP

#include <cstdio>
#include <string>

#include "../types.hpp"

#ifdef SYNAO_DEBUG_BUILD
	#define SYNAO_LOG(...) do { sint_t _r = std::printf(__VA_ARGS__); assert(_r >= 0); } while(false)
#else
	#define SYNAO_LOG(...)
#endif // SYNAO_DEBUG_BUILD

#endif // LEVIATHAN_INCLUDED_UTILITY_LOGGER_HPP
