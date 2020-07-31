#ifndef SYNAO_AUDIO_ALCHECK_HPP
#define SYNAO_AUDIO_ALCHECK_HPP

#ifdef SYNAO_WITHOUT_VCPKG // FUCK
	#include <al.h>
	#include <alc.h>
#else // SYNAO_WITHOUT_VCPKG
	#include <AL/al.h>
	#include <AL/alc.h>
#endif // SYNAO_WITHOUT_VCPKG

#include "../types.hpp"

#ifdef SYNAO_DEBUG_BUILD
namespace priv {
	void alCheckError(const byte_t* file, uint_t line, const byte_t* expression);
}
#define alCheck(expr) do { expr; priv::alCheckError(__FILE__, __LINE__, #expr); } while(false)
#else // SYNAO_DEBUG_BUILD
#define alCheck(expr) (expr)
#endif // SYNAO_DEBUG_BUILD

#endif // SYNAO_AUDIO_ALCHECK_HPP
