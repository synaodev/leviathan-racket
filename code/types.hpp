#ifndef SYNAO_TYPES_HPP
#define SYNAO_TYPES_HPP

#include <cstdint>

#include <glm/common.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/trigonometric.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/vec1.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

struct not_copyable_t {
public:
	not_copyable_t() = default;
	not_copyable_t(not_copyable_t&&) = default;
	not_copyable_t& operator=(not_copyable_t&&) = default;
	not_copyable_t(const not_copyable_t&) = delete;
	not_copyable_t& operator=(const not_copyable_t&) = delete;
};

using sint8_t  = signed char;
// using uint8  = unsigned char;
using bool16_t = short;
using sint16_t = signed short;
// using uint16 = unsigned short;
using bool32_t = int;
using sint32_t = signed int;
// using uint32 = unsigned int;
using real32_t = float;
using bool64_t = long long;
using sint64_t = signed long long;
// using uint64 = unsigned long long;
using real64_t = double;

using byte_t = char;
using bool_t = int;
using sint_t = signed int;
using uint_t = unsigned int;
using real_t = float;
using void_t = void;
using optr_t = void*;

#if defined(_WIN32) && !defined(_WIN64)
#define SYNAO_MACHINE_x86
using arch_t = uint32_t;
#else // _WIN32 _WIN64
#define SYNAO_MACHINE_x64
using arch_t = uint64_t;
#endif // _WIN32 _WIN64

#ifndef NDEBUG
#define SYNAO_DEBUG_BUILD
#endif // NDEBUG

#ifndef __GNUC__
#ifndef and
#define and &&
#endif // and

#ifndef or
#define or ||
#endif // or
#endif // __GNUC__

namespace rng {
	sint_t next(sint_t low, sint_t high);
	real_t next(real_t low, real_t high);
}

namespace interval {
	constexpr real64_t kMin = 1.0 / 60.0;
	constexpr real64_t kMax = 1.0 / 62.0;
}

#endif // SYNAO_TYPES_HPP