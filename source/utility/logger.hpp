#ifndef LEVIATHAN_INCLUDED_UTILITY_LOGGER_HPP
#define LEVIATHAN_INCLUDED_UTILITY_LOGGER_HPP

#include <cstdio>
#include <string>

#include "../types.hpp"

#ifdef LEVIATHAN_BUILD_DEBUG
	#define synao_log(...) do { sint_t _r = std::printf(__VA_ARGS__); assert(_r >= 0); } while(false)
#else
	#define synao_log(...)
#endif

#endif // LEVIATHAN_INCLUDED_UTILITY_LOGGER_HPP
