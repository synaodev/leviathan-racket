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
// using uint8_t  = unsigned char;
using bool16_t = short;
using sint16_t = signed short;
// using uint16_t = unsigned short;
using bool32_t = int;
using sint32_t = signed int;
// using uint32_t = unsigned int;
using real32_t = float;
using bool64_t = long long;
using sint64_t = signed long long;
// using uint64_t = unsigned long long;
using real64_t = double;

using byte_t = char;
using bool_t = int;
using sint_t = signed int;
using uint_t = unsigned int;
using real_t = float;
using void_t = void;
using optr_t = void*;

#if defined(TARGET_VOIDP_EQUALS_64)
	#define SYNAO_MACHINE_x64
	using arch_t = uint64_t;
#elif defined(TARGET_VOIDP_EQUALS_32)
	#define SYNAO_MACHINE_x86
	using arch_t = uint32_t;
#else
	#error "Error! Architecture is undefined!"
#endif // TARGET_VOIDP_EQUALS*

#if !defined(NDEBUG) || defined(_DEBUG) || defined(DEBUG)
	#define SYNAO_DEBUG_BUILD
#endif // NDEBUG _DEBUG DEBUG

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

#endif // SYNAO_TYPES_HPP
