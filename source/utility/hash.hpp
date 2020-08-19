#ifndef LEVIATHAN_INCLUDED_UTILITY_HASH_HPP
#define LEVIATHAN_INCLUDED_UTILITY_HASH_HPP

#include "../types.hpp"

namespace priv {
	inline constexpr arch_t synao_hash_impl(const byte_t* const key) {
	#ifdef LEVIATHAN_MACHINE_64BIT
		arch_t value = 0xcbf29ce484222325;
		arch_t prime = 0x100000001b3;
	#else
		arch_t value = 0x811c9dc5;
		arch_t prime = 0x1000193;
	#endif
		for (arch_t it = 0; key[it] != '\0'; ++it) {
			value ^= static_cast<arch_t>(key[it]);
			value *= prime;
		}
		return value;
	}
}

#ifdef LEVIATHAN_TOOLCHAIN_MSVC
	#define synao_hash(KEY)					\
		__pragma(warning(push))				\
		__pragma(warning(disable: 4307))	\
		priv::synao_hash_impl(KEY)			\
		__pragma(warning(pop))
#else
	#define synao_hash(KEY) priv::synao_hash_impl(KEY)
#endif

#endif // LEVIATHAN_INCLUDED_UTILITY_HASH_HPP
