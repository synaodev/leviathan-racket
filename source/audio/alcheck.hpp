#ifndef LEVIATHAN_INCLUDED_AUDIO_ALCHECK_HPP
#define LEVIATHAN_INCLUDED_AUDIO_ALCHECK_HPP

#include "../types.hpp"

#if defined(LEVIATHAN_PLATFORM_LINUX) || defined(LEVIATHAN_USES_VCPKG)
	#include <AL/al.h>
	#include <AL/alc.h>
#else
	#include <al.h>
	#include <alc.h>
#endif

#ifdef LEVIATHAN_BUILD_DEBUG
	namespace priv {
		void alCheckError(const byte_t* file, uint_t line, const byte_t* expression);
	}
	#define alCheck(expr) do { expr; priv::alCheckError(__FILE__, __LINE__, #expr); } while(false)
#else
	#define alCheck(expr) (expr)
#endif

#endif // LEVIATHAN_INCLUDED_AUDIO_ALCHECK_HPP
