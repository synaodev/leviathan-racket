#pragma once

#include "../types.hpp"

namespace frame_buffer {
	void clear(const glm::ivec2& dimensions, const glm::vec4& color);
	void clear(const glm::ivec2& dimensions);
	void viewport(const glm::ivec2& dimensions);
	void bucket(const glm::vec4& color);
}
