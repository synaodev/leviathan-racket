#ifndef SYNAO_GRAPHICS_GLCHECK_HPP
#define SYNAO_GRAPHICS_GLCHECK_HPP

#include "../types.hpp"

#include "./glad.hpp"

#ifdef SYNAO_DEBUG_BUILD
namespace priv {
	void glCheckError(const byte_t* file, uint_t line, const byte_t* expression);
}
#define glCheck(expr) do { expr; priv::glCheckError(__FILE__, __LINE__, #expr); } while(false)
#else // SYNAO_DEBUG_BUILD
#define glCheck(expr) (expr)
#endif // SYNAO_DEBUG_BUILD

#ifdef glTexStorage3D
#define SYNAO_IS_GL_420 glTexStorage3D
#endif // glTexStorage3D

#endif // SYNAO_GRAPHICS_GLCHECK_HPP