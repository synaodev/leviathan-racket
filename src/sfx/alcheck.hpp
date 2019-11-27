#ifndef SYNAO_SOUND_ALCHECK_HPP
#define SYNAO_SOUND_ALCHECK_HPP

#include <AL/al.h>
#include <AL/alc.h>

#include "../types.hpp"

#ifdef SYNAO_DEBUG_BUILD
namespace priv {
	void alCheckError(const byte_t* file, uint_t line, const byte_t* expression);
}
#define alCheck(expr) do { expr; priv::alCheckError(__FILE__, __LINE__, #expr); } while(false)
#else // SYNAO_DEBUG_BUILD
#define alCheck(expr) (expr)
#endif // SYNAO_DEBUG_BUILD

#endif // SYNAO_SOUND_ALCHECK_HPP