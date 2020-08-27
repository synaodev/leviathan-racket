#ifndef LEVIATHAN_INCLUDED_AUDIO_ALCHECK_HPP
#define LEVIATHAN_INCLUDED_AUDIO_ALCHECK_HPP

#ifndef LEVIATHAN_USES_VCPKG
	#include <al.h>
	#include <alc.h>
#else
	#include <AL/al.h>
	#include <AL/alc.h>
#endif

#include "../types.hpp"

#ifdef LEVIATHAN_BUILD_DEBUG
	namespace priv {
		void alCheckError(const byte_t* file, uint_t line, const byte_t* expression);
	}
	#define alCheck(expr) do { expr; priv::alCheckError(__FILE__, __LINE__, #expr); } while(false)
#else
	#define alCheck(expr) (expr)
#endif

#endif // LEVIATHAN_INCLUDED_AUDIO_ALCHECK_HPP
