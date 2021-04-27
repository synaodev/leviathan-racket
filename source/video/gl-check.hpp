#pragma once

#include <glad/glad.h>
#include <glm/vec2.hpp>

#include "../types.hpp"

extern glm::ivec2 opengl_version;

#ifdef LEVIATHAN_BUILD_DEBUG
	namespace priv {
		void glCheckError(const byte_t* file, uint_t line, const byte_t* expression);
	}
	#define glCheck(expr) do { expr; priv::glCheckError(__FILE__, __LINE__, #expr); } while(false)
#else
	#define glCheck(expr) (expr)
#endif
