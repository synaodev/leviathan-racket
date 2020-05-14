#ifndef SYNAO_VIDEO_GLCHECK_HPP
#define SYNAO_VIDEO_GLCHECK_HPP

#include "../types.hpp"

#ifndef __EMSCRIPTEN__
#include "./glad.hpp"
#else // __EMSCRIPTEN__
#include <SDL2/SDL_opengles2.h>
#include <GLES3/gl3.h>
#endif // __EMSCRIPTEN__

#ifdef SYNAO_DEBUG_BUILD
namespace priv {
	void glCheckError(const byte_t* file, uint_t line, const byte_t* expression);
}
#define glCheck(expr) do { expr; priv::glCheckError(__FILE__, __LINE__, #expr); } while(false)
#else // SYNAO_DEBUG_BUILD
#define glCheck(expr) (expr)
#endif // SYNAO_DEBUG_BUILD

#endif // SYNAO_VIDEO_GLCHECK_HPP