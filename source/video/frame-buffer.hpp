#ifndef LEVIATHAN_INCLUDED_VIDEO_FRAME_BUFFER_HPP
#define LEVIATHAN_INCLUDED_VIDEO_FRAME_BUFFER_HPP

#include "../types.hpp"

namespace frame_buffer {
	void clear(glm::ivec2 dimensions, glm::vec4 color);
	void clear(glm::ivec2 dimensions);
	void viewport(glm::ivec2 dimensions);
	void bucket(glm::vec4 color);
}

#endif // LEVIATHAN_INCLUDED_VIDEO_FRAME_BUFFER_HPP
