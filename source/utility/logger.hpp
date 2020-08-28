#ifndef LEVIATHAN_INCLUDED_UTILITY_LOGGER_HPP
#define LEVIATHAN_INCLUDED_UTILITY_LOGGER_HPP

#include "../types.hpp"

#ifdef LEVIATHAN_BUILD_DEBUG
	#include <fmt/core.h>
	#define synao_log(...) fmt::print(__VA_ARGS__)
#else
	#define synao_log(...)
#endif

#endif // LEVIATHAN_INCLUDED_UTILITY_LOGGER_HPP
