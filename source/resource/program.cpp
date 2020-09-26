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
layout(location = 2) in vec2 uvs;
layout(location = 3) in float alpha;
layout(location = 4) in int texID;
layout(location = 5) in int palID;
out STAGE {
	layout(location = 0) vec2 uvs;
	layout(location = 1) float alpha;
	layout(location = 2) int texID;
	layout(location = 3) int palID;
} vs;
void main() {
	gl_Position = viewports[matrix] * vec4(position, 0.0f, 1.0f);
	vs.uvs = uvs;
	vs.alpha = alpha;
	vs.texID = texID;
	vs.palID = palID;
})";

static constexpr byte_t kMajorVert330[] = R"(#version 330 core
layout(std140) uniform transforms {
	mat4 viewports[2];
};
layout(location = 0) in vec2 position;
layout(location = 1) in int matrix;
layout(location = 2) in vec2 uvs;
layout(location = 3) in float alpha;
layout(location = 4) in int texID;
layout(location = 5) in int palID;
out STAGE {
	vec2 uvs;
	float alpha;
	int texID;
	int palID;
} vs;
void main() {
	gl_Position = viewports[matrix] * vec4(position, 0.0f, 1.0f);
	vs.uvs = uvs;
	vs.alpha = alpha;
	vs.texID = texID;
	vs.palID = palID;
})";

static constexpr byte_t kFontsVert420[] = R"(#version 420 core
layout(std140) uniform transforms {
	mat4 viewports[2];
};
layout(location = 0) in vec2 position;
layout(location = 1) in int matrix;
layout(location = 2) in vec2 uvs;
layout(location = 3) in vec4 color;
layout(location = 4) in int texID;
layout(location = 5) in int tblID;
out STAGE {
	layout(location = 0) vec2 uvs;
	layout(location = 1) vec4 color;
	layout(location = 2) int texID;
	layout(location = 3) int tblID;
} vs;
void main() {
	gl_Position = viewports[matrix] * vec4(position, 0.0f, 1.0f);
	vs.uvs = uvs;
	vs.color = color;
	vs.texID = texID;
	vs.tblID = tblID;
})";

static constexpr byte_t kFontsVert330[] = R"(#version 330 core
layout(std140) uniform transforms {
	mat4 viewports[2];
};
layout(location = 0) in vec2 position;
layout(location = 1) in int matrix;
layout(location = 2) in vec2 uvs;
layout(location = 3) in vec4 color;
layout(location = 4) in int texID;
layout(location = 5) in int tblID;
out STAGE {
	vec2 uvs;
	vec4 color;
	int texID;
	int tblID;
} vs;
void main() {
	gl_Position = viewports[matrix] * vec4(position, 0.0f, 1.0f);
	vs.uvs = uvs;
	vs.color = color;
	vs.texID = texID;
	vs.tblID = tblID;
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
layout(binding = 0) uniform sampler2DArray diffuse_map;
in STAGE {
	layout(location = 0) vec2 uvs;
	layout(location = 1) float alpha;
	layout(location = 2) int texID;
	layout(location = 3) int palID;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 color = texture(diffuse_map, vec3(fs.uvs, float(fs.texID)));
	fragment = vec4(color.rgb, color.a * fs.alpha);
})";

static constexpr byte_t kSpritesFrag330[] = R"(#version 330 core
uniform sampler2DArray diffuse_map;
in STAGE {
	vec2 uvs;
	float alpha;
	int texID;
	int palID;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 color = texture(diffuse_map, vec3(fs.uvs, float(fs.texID)));
	fragment = vec4(color.rgb, color.a * fs.alpha);
})";

static constexpr byte_t kIndexedFrag420[] = R"(#version 420 core
layout(binding = 0) uniform sampler2DArray indexed_map;
layout(binding = 1) uniform sampler1DArray palette_map;
in STAGE {
	layout(location = 0) vec2 uvs;
	layout(location = 1) float alpha;
	layout(location = 2) int texID;
	layout(location = 3) int palID;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 pixel = texture(indexed_map, vec3(fs.uvs, float(fs.texID)));
	vec4 color = texture(palette_map, vec2(pixel[0], float(fs.palID)));
	fragment = vec4(color.rgb, color.a * fs.alpha);
})";

static constexpr byte_t kIndexedFrag330[] = R"(#version 330 core
uniform sampler2DArray indexed_map;
uniform sampler1DArray palette_map;
in STAGE {
	vec2 uvs;
	float alpha;
	int texID;
	int palID;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 pixel = texture(indexed_map, vec3(fs.uvs, float(fs.texID)));
	vec4 color = texture(palette_map, vec2(pixel[0], float(fs.palID)));
	fragment = vec4(color.rgb, color.a * fs.alpha);
})";

static constexpr byte_t kChannelsFrag420[] = R"(#version 420 core
layout(binding = 0) uniform sampler2DArray channels_map;
in STAGE {
	layout(location = 0) vec2 uvs;
	layout(location = 1) vec4 color;
	layout(location = 2) int texID;
	layout(location = 3) int tblID;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 pixel = texture(channels_map, vec3(fs.uvs, float(fs.texID)));
	fragment = vec4(vec3(pixel[tblID]), 1.0f) * fs.color;
})";

static constexpr byte_t kChannelsFrag330[] = R"(#version 330 core
uniform sampler2DArray channels_map;
in STAGE {
	vec2 uvs;
	vec4 color;
	int texID;
	int tblID;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 pixel = texture(channels_map, vec3(fs.uvs, float(fs.texID)));
	fragment = vec4(vec3(pixel[tblID]), 1.0f) * fs.color;
})";

namespace program {
	std::string directive(const glm::ivec2& version) {
		return fmt::format(
			"#version {}{}0",
			version[0],
			version[1]
		);
	}
	std::string blank_vert(const glm::ivec2& version) {
		if (version[0] == 4 and version[1] >= 2) {
			return kBlankVert420;
		}
		return kBlankVert330;
	}
	std::string major_vert(const glm::ivec2& version) {
		if (version[0] == 4 and version[1] >= 2) {
			return kMajorVert420;
		}
		return kMajorVert330;
	}
	std::string fonts_vert(const glm::ivec2& version) {
		if (version[0] == 4 and version[1] >= 2) {
			return kFontsVert420;
		}
		return kFontsVert330;
	}
	std::string colors_frag(const glm::ivec2& version) {
		if (version[0] == 4 and version[1] >= 2) {
			return kColorsFrag420;
		}
		return kColorsFrag330;
	}
	std::string sprites_frag(const glm::ivec2& version) {
		if (version[0] == 4 and version[1] >= 2) {
			return kSpritesFrag420;
		}
		return kSpritesFrag330;
	}
	std::string indexed_frag(const glm::ivec2& version) {
		if (version[0] == 4 and version[1] >= 2) {
			return kIndexedFrag420;
		}
		return kIndexedFrag330;
	}
	std::string channels_frag(const glm::ivec2& version) {
		if (version[0] == 4 and version[1] >= 2) {
			return kChannelsFrag420;
		}
		return kChannelsFrag330;
	}
}
