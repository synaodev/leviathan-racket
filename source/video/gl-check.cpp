#include "./gl-check.hpp"

#ifdef LEVIATHAN_BUILD_DEBUG
#include "../utility/logger.hpp"

void priv::glCheckError(const byte_t* file, uint_t line, const byte_t* expression) {
	GLenum code = glGetError();
	if (code != GL_NO_ERROR) {
		const byte_t* error = "Unknown GL Error!";
		const byte_t* description = "No description possible.";
		switch (code) {
		case GL_INVALID_ENUM:
			error = "GL_INVALID_ENUM";
			description = "An unacceptable value has been specified for an enumerated argument.";
			break;
		case GL_INVALID_VALUE:
			error = "GL_INVALID_VALUE";
			description = "A numeric argument is out of range.";
			break;
		case GL_INVALID_OPERATION:
			error = "GL_INVALID_OPERATION";
			description = "The specified operation is not allowed in the current state.";
			break;
		case GL_OUT_OF_MEMORY:
			error = "GL_OUT_OF_MEMORY";
			description = "There is not enough memory left to execute the command.";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			error = "GL_INVALID_FRAMEBUFFER_OPERATION";
			description = "The object bound to FRAMEBUFFER_BINDING is not \"framebuffer complete\".";
			break;
		default:
			break;
		}
		std::string fstr = file;
		synao_log(
			"An internal OpenGL call failed in {} ({})!\n"
			"Expression:\n\t{}\n"
			"Error description:\n\t{}\n\t{}\n",
			fstr.substr(fstr.find_last_of("\\/") + 1),
			line, expression, error, description
		);
		sint_t i = 0;
	}
}

#endif
