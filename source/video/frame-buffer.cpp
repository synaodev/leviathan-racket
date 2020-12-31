#include "./frame-buffer.hpp"
#include "./gl-check.hpp"

#include <glm/vec4.hpp>
#include <glm/gtc/constants.hpp>

void frame_buffer::clear(glm::ivec2 dimensions, glm::vec4 color) {
	frame_buffer::viewport(dimensions);
	frame_buffer::bucket(color);
	glCheck(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void frame_buffer::clear(glm::ivec2 dimensions) {
	frame_buffer::clear(dimensions, glm::zero<glm::vec4>());
}

void frame_buffer::viewport(glm::ivec2 dimensions) {
	static glm::ivec2 current = glm::zero<glm::ivec2>();
	if (current != dimensions) {
		current = dimensions;
		glCheck(glViewport(0, 0, dimensions.x, dimensions.y));
	}
}

void frame_buffer::bucket(glm::vec4 color) {
	static glm::vec4 current = glm::zero<glm::vec4>();
	if (current != color) {
		current = color;
		glCheck(glClearColor(color.x, color.y, color.z, color.w));
	}
}
