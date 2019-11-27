#ifndef SYNAO_UTILITY_HASH_HPP
#define SYNAO_UTILITY_HASH_HPP

#include "../types.hpp"

namespace priv {
	inline constexpr arch_t hash(const byte_t* const key) {
#ifdef SYNAO_MACHINE_x64
		arch_t value = 0xcbf29ce484222325;
		arch_t prime = 0x100000001b3;
#else // SYNAO_MACHINE_x64
		arch_t value = 0x811c9dc5;
		arch_t prime = 0x1000193;
#endif // SYNAO_MACHINE_x64
		for (arch_t it = 0; key[it] != '\0'; ++it) {
			value ^= static_cast<arch_t>(key[it]);
			value *= prime;
		}
		return value;
	}
}

#ifdef _MSC_VER
#define SYNAO_HASH(KEY)					\
	__pragma(warning(push))				\
	__pragma(warning(disable: 4307))	\
	priv::hash(KEY)						\
	__pragma(warning(pop))
#else
#define SYNAO_HASH(KEY) priv::hash(KEY)
#endif // _MSC_VER

#endif // SYNAO_UTILITY_HASH_HPP