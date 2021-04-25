#include "./al-check.hpp"

#ifdef LEVIATHAN_BUILD_DEBUG
#include "../utility/logger.hpp"

void priv::alCheckError(const byte_t* file, uint_t line, const byte_t* expression) {
	ALenum code = alGetError();
	if (code != AL_NO_ERROR) {
		const byte_t* error = "Unknown AL Error!";
		const byte_t* description = "No description possible.";
		switch (code) {
		case AL_INVALID_NAME:
			error = "AL_INVALID_NAME";
			description = "A bad name (ID) has been specified.";
			break;
		case AL_INVALID_ENUM:
			error = "AL_INVALID_ENUM";
			description = "An unacceptable value has been specified for an enumerated argument.";
			break;
		case AL_INVALID_VALUE:
			error = "AL_INVALID_VALUE";
			description = "A numeric argument is out of range.";
			break;
		case AL_INVALID_OPERATION:
			error = "AL_INVALID_OPERATION";
			description = "The specified operation is not allowed in the current state.";
			break;
		case AL_OUT_OF_MEMORY:
			error = "AL_OUT_OF_MEMORY";
			description = "There is not enough memory left to execute the command.";
			break;
		default:
			break;
		}
		const std::string fstr = file;
		synao_log(
			"An internal OpenAL call failed in {} ({})!\n"
			"Expression:\n\t{}\n"
			"Error description:\n\t{}\n\t{}\n",
			fstr.substr(fstr.find_last_of("\\/") + 1),
			line, expression, error, description
		);
	}
}

#endif
