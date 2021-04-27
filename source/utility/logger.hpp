#pragma once

#include "../types.hpp"

#ifdef LEVIATHAN_BUILD_DEBUG
	#include <fmt/core.h>
	#define synao_log(...) fmt::print(__VA_ARGS__)
#else
	#define synao_log(...)
#endif
