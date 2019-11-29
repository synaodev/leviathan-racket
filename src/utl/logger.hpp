#ifndef SYNAO_UTILITY_LOGGER_HPP
#define SYNAO_UTILITY_LOGGER_HPP

#include "../types.hpp"

#ifdef SYNAO_DEBUG_BUILD

#include <cstdio>
#include <string>

#define SYNAO_LOG(...) do { sint_t r = std::printf(__VA_ARGS__); assert(r >= 0); } while(false)
#else // SYNAO_DEBUG_BUILD
#define SYNAO_LOG(...) 
#endif // SYNAO_DEBUG_BUILD

#endif // SYNAO_UTILITY_LOGGER_HPP