#include "./program.hpp"

#ifndef FMT_HEADER_ONLY
	#define FMT_HEADER_ONLY
#endif // FMT_HEADER_ONLY
#include <fmt/format.h>

static constexpr byte_t kBlankVert420[] = R"(#version 420 core
layout(binding = 0, std140) uniform transforms {
	mat4 viewports[2];
};
layout(location = 0) in vec2 position;
layout(location = 1) in int matrix;
layout(location = 2) in vec4 color;
out STAGE {
	layout(location = 0) vec4 color;
} vs;
void main() {
	gl_Position = viewports[matrix] * vec4(position, 0.0f, 1.0f);
	vs.color = color;
})";

static constexpr byte_t kBlankVert330[] = R"(#version 330 core
layout(std140) uniform transforms {
	mat4 viewports[2];
};
layout(location = 0) in vec2 position;
layout(location = 1) in int matrix;
layout(location = 2) in vec4 color;
out STAGE {
	vec4 color;
} vs;
void main() {
	gl_Position = viewports[matrix] * vec4(position, 0.0f, 1.0f);
	vs.color = color;
})";

static constexpr byte_t kMajorVert420[] = R"(#version 420 core
layout(binding = 0, std140) uniform transforms {
	mat4 viewports[2];
};
layout(location = 0) in vec2 position;
layout(location = 1) in int matrix;
layout(location = 2) in vec3 uvs;
layout(location = 3) in float alpha;
out STAGE {
	layout(location = 0) vec3 uvs;
	layout(location = 1) float alpha;
} vs;
void main() {
	gl_Position = viewports[matrix] * vec4(position, 0.0f, 1.0f);
	vs.uvs = uvs;
	vs.alpha = alpha;
})";

static constexpr byte_t kMajorVert330[] = R"(#version 330 core
layout(std140) uniform transforms {
	mat4 viewports[2];
};
layout(location = 0) in vec2 position;
layout(location = 1) in int matrix;
layout(location = 2) in vec3 uvs;
layout(location = 3) in float alpha;
out STAGE {
	vec3 uvs;
	float alpha;
} vs;
void main() {
	gl_Position = viewports[matrix] * vec4(position, 0.0f, 1.0f);
	vs.uvs = uvs;
	vs.alpha = alpha;
})";

static constexpr byte_t kColorsFrag420[] = R"(#version 420 core
in STAGE {
	layout(location = 0) vec4 color;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	fragment = fs.color;
})";

static constexpr byte_t kColorsFrag330[] = R"(#version 330 core
in STAGE {
	vec4 color;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	fragment = fs.color;
})";

static constexpr byte_t kSpritesFrag420[] = R"(#version 420 core
layout(binding = 0) uniform sampler2D diffuse_map;
in STAGE {
	layout(location = 0) vec3 uvs;
	layout(location = 1) float alpha;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 color = texture(diffuse_map, fs.uvs.xy);
	fragment = vec4(color.rgb, color.a * fs.alpha);
})";

static constexpr byte_t kSpritesFrag330[] = R"(#version 330 core
uniform sampler2D diffuse_map;
in STAGE {
	vec3 uvs;
	float alpha;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 color = texture(diffuse_map, fs.uvs.xy);
	fragment = vec4(color.rgb, color.a * fs.alpha);
})";

static constexpr byte_t kIndexedFrag420[] = R"(#version 420 core
layout(binding = 0) uniform sampler2D indexed_map;
layout(binding = 1) uniform sampler2D palette_map;
in STAGE {
	layout(location = 0) vec3 uvs;
	layout(location = 1) float alpha;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 index = texture(indexed_map, fs.uvs.xy);
	vec4 color = texture(palette_map, vec2(index[0], fs.uvs.z));
	fragment = vec4(color.rgb, color.a * fs.alpha);
})";

static constexpr byte_t kIndexedFrag330[] = R"(#version 330 core
uniform sampler2D indexed_map;
uniform sampler2D palette_map;
in STAGE {
	vec3 uvs;
	float alpha;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 index = texture(indexed_map, fs.uvs.xy);
	vec4 color = texture(palette_map, vec2(index[0], fs.uvs.z));
	fragment = vec4(color.rgb, color.a * fs.alpha);
})";

namespace program {
	std::string directive(glm::ivec2 version) {
		return fmt::format(
			"#version {}{}0",
			version[0],
			version[1]
		);
	}
	std::string blank_vert(glm::ivec2 version) {
		if (version[0] == 4 and version[1] >= 2) {
			return kBlankVert420;
		}
		return kBlankVert330;
	}
	std::string major_vert(glm::ivec2 version) {
		if (version[0] == 4 and version[1] >= 2) {
			return kMajorVert420;
		}
		return kMajorVert330;
	}
	std::string colors_frag(glm::ivec2 version) {
		if (version[0] == 4 and version[1] >= 2) {
			return kColorsFrag420;
		}
		return kColorsFrag330;
	}
	std::string sprites_frag(glm::ivec2 version) {
		if (version[0] == 4 and version[1] >= 2) {
			return kSpritesFrag420;
		}
		return kSpritesFrag330;
	}
	std::string indexed_frag(glm::ivec2 version) {
		if (version[0] == 4 and version[1] >= 2) {
			return kIndexedFrag420;
		}
		return kIndexedFrag330;
	}
}
