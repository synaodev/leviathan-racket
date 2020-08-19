#ifndef LEVIATHAN_INCLUDED_AUDIO_ALCHECK_HPP
#define LEVIATHAN_INCLUDED_AUDIO_ALCHECK_HPP

#ifdef TARGET_TOOLCHAIN_VCPKG
	#include <al.h>
	#include <alc.h>
#else // TARGET_TOOLCHAIN_VCPKG
	#include <AL/al.h>
	#include <AL/alc.h>
#endif // TARGET_TOOLCHAIN_VCPKG

#include "../types.hpp"

#ifdef SYNAO_DEBUG_BUILD
	namespace priv {
		void alCheckError(const byte_t* file, uint_t line, const byte_t* expression);
	}
	#define alCheck(expr) do { expr; priv::alCheckError(__FILE__, __LINE__, #expr); } while(false)
#else // SYNAO_DEBUG_BUILD
	#define alCheck(expr) (expr)
#endif // SYNAO_DEBUG_BUILD

#endif // LEVIATHAN_INCLUDED_AUDIO_ALCHECK_HPP
