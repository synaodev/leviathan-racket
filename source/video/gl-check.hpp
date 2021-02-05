#ifndef LEVIATHAN_INCLUDED_VIDEO_GL_CHECK_HPP
#define LEVIATHAN_INCLUDED_VIDEO_GL_CHECK_HPP

#include <glad/glad.h>
#include <glm/vec2.hpp>

#include "../types.hpp"

extern glm::ivec2 opengl_version;
extern bool_t opengl_nvidia_card;

#ifdef LEVIATHAN_BUILD_DEBUG
	namespace priv {
		void glCheckError(const byte_t* file, uint_t line, const byte_t* expression);
	}
	#define glCheck(expr) do { expr; priv::glCheckError(__FILE__, __LINE__, #expr); } while(false)
#else
	#define glCheck(expr) (expr)
#endif

#endif // LEVIATHAN_INCLUDED_VIDEO_GL_CHECK_HPP
