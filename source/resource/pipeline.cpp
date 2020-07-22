#include "./pipeline.hpp"

static constexpr byte_t kMinorVert420[] = R"("
#version 420 core
layout(location = 0) in vec2 position;
void main() {
	gl_Position = vec4(position, 0.0f, 1.0f);
})";

static constexpr byte_t kMinorVert330[] = R"("
#version 330 core
layout(location = 0) in vec2 position;
void main() {
	gl_Position = vec4(position, 0.0f, 1.0f);
})";

static constexpr byte_t kMinorVertGLES[] = R"("
#version 300 es
layout(location = 0) in vec2 position;
layout(location = 0) out vec4 vs_color;
void main() {
	gl_Position = vec4(position, 0.0f, 1.0f);
	vs_color = vec4(1.0f);
})";

std::string pipeline::minor_vert(glm::ivec2 version) {
	if (version[0] == 4 and version[1] >= 2) {
		return kMinorVert420;
	}
	return kMinorVert330;
}

static constexpr byte_t kBlankVert420[] = R"(
#version 420 core
layout(binding = 0, std140) uniform transforms {
	mat4 viewport;
	vec2 dimensions;
	vec2 resolution;
};
layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
out STAGE {
	layout(location = 0) vec4 color;
} vs;
void main() {
	gl_Position = viewport * vec4(position, 0.0f, 1.0f);
	vs.color = color;
})";

static constexpr byte_t kBlankVert330[] = R"(
#version 330 core
layout(std140) uniform transforms {
	mat4 viewport;
	vec2 dimensions;
	vec2 resolution;
};
layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
out STAGE {
	vec4 color;
} vs;
void main() {
	gl_Position = viewport * vec4(position, 0.0f, 1.0f);
	vs.color = color;
})";

static constexpr byte_t kBlankVertGLES[] = R"(
#version 300 es
layout(binding = 0, std140) uniform transforms {
	mat4 viewport;
	vec2 dimensions;
	vec2 resolution;
};
layout(location = 0) in vec2 position;
layout(location = 1) in vec4 color;
layout(location = 0) out vec4 vs_color;
void main() {
	gl_Position = viewport * vec4(position, 0.0f, 1.0f);
	vs_color = color;
})";

std::string pipeline::blank_vert(glm::ivec2 version) {
	if (version[0] == 4 and version[1] >= 2) {
		return kBlankVert420;
	}
	return kBlankVert330;
}

static constexpr byte_t kMajorVert420[] = R"(
#version 420 core
layout(binding = 0, std140) uniform transforms {
	mat4 viewport;
	vec2 dimensions;
	vec2 resolution;
};
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 uvcoords;
layout(location = 2) in float alpha;
out STAGE {
	layout(location = 0) vec3 uvcoords;
	layout(location = 1) float alpha;
} vs;
void main() {
	gl_Position = viewport * vec4(position, 0.0f, 1.0f);
	vs.uvcoords = uvcoords;
	vs.alpha = alpha;
})";

static constexpr byte_t kMajorVert330[] = R"(
#version 330 core
layout(std140) uniform transforms {
	mat4 viewport;
	vec2 dimensions;
	vec2 resolution;
};
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 uvcoords;
layout(location = 2) in float alpha;
out STAGE {
	vec3 uvcoords;
	float alpha;
} vs;
void main() {
	gl_Position = viewport * vec4(position, 0.0f, 1.0f);
	vs.uvcoords = uvcoords;
	vs.alpha = alpha;
})";

static constexpr byte_t kMajorVertGLES[] = R"(
#version 330 core
layout(binding = 0, std140) uniform transforms {
	mat4 viewport;
	vec2 dimensions;
	vec2 resolution;
};
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 uvcoords;
layout(location = 2) in float alpha;
layout(location = 0) out vec3 vs_uvcoords;
layout(location = 1) out float vs_alpha;
void main() {
	gl_Position = viewport * vec4(position, 0.0f, 1.0f);
	vs_uvcoords = uvcoords;
	vs_alpha = alpha;
})";

std::string pipeline::major_vert(glm::ivec2 version) {
	if (version[0] == 4 and version[1] >= 2) {
		return kMajorVert420;
	}
	return kMajorVert330;
}

static constexpr byte_t kColorsFrag420[] = R"(
#version 420 core
in STAGE {
	layout(location = 0) vec4 color;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	fragment = fs.color;
})";

static constexpr byte_t kColorsFrag330[] = R"(
#version 330 core
in STAGE {
	vec4 color;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	fragment = fs.color;
})";

static constexpr byte_t kColorsFragGLES[] = R"(
#version 300 es
layout(location = 0) in vec4 fs_color;
layout(location = 0) out vec4 fragment;
void main() {
	fragment = fs_color;
})";

std::string pipeline::colors_frag(glm::ivec2 version) {
	if (version[0] == 4 and version[1] >= 2) {
		return kColorsFrag420;
	}
	return kColorsFrag330;
}

static constexpr byte_t kSpritesFrag420[] = R"(
#version 420 core
layout(binding = 0) uniform sampler2D diffuse_map;
in STAGE {
	layout(location = 0) vec3 uvcoords;
	layout(location = 1) float alpha;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 color = texture(diffuse_map, fs.uvcoords.xy);
	fragment = vec4(color.rgb, color.a * fs.alpha);
})";

static constexpr byte_t kSpritesFrag330[] = R"(
#version 330 core
uniform sampler2D diffuse_map;
in STAGE {
	vec3 uvcoords;
	float alpha;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 color = texture(diffuse_map, fs.uvcoords.xy);
	fragment = vec4(color.rgb, color.a * fs.alpha);
})";

static constexpr byte_t kSpritesFragGLES[] = R"(
#version 330 core
layout(binding = 0) uniform sampler2D diffuse_map;
layout(location = 0) in vec3 fs_uvcoords;
layout(location = 1) in float fs_alpha;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 color = texture(diffuse_map, fs_uvcoords.xy);
	fragment = vec4(color.rgb, color.a * fs_alpha);
})";

std::string pipeline::sprites_frag(glm::ivec2 version) {
	if (version[0] == 4 and version[1] >= 2) {
		return kSpritesFrag420;
	}
	return kSpritesFrag330;
}

static constexpr byte_t kIndexedFrag420[] = R"(
#version 420 core
layout(binding = 0) uniform sampler2D indexed_map;
layout(binding = 1) uniform sampler2D palette_map;
in STAGE {
	layout(location = 0) vec3 uvcoords;
	layout(location = 1) float alpha;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 index = texture(indexed_map, fs.uvcoords.xy);
	vec4 color = texture(palette_map, vec2(index[0], fs.uvcoords.z));
	fragment = vec4(color.rgb, color.a * fs.alpha);
})";

static constexpr byte_t kIndexedFrag330[] = R"(
#version 330 core
uniform sampler2D indexed_map;
uniform sampler2D palette_map;
in STAGE {
	vec3 uvcoords;
	float alpha;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 index = texture(indexed_map, fs.uvcoords.xy);
	vec4 color = texture(palette_map, vec2(index[0], fs.uvcoords.z));
	fragment = vec4(color.rgb, color.a * fs.alpha);
})";

static constexpr byte_t kIndexedFragGLES[] = R"(
#version 330 core
layout(binding = 0) uniform sampler2D indexed_map;
layout(binding = 1) uniform sampler2D palette_map;
layout(location = 0) in vec3 fs_uvcoords;
layout(location = 1) in float fs_alpha;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 index = texture(indexed_map, fs_uvcoords.xy);
	vec4 color = texture(palette_map, vec2(index[0], fs_uvcoords.z));
	fragment = vec4(color.rgb, color.a * fs_alpha);
})";

std::string pipeline::indexed_frag(glm::ivec2 version) {
	if (version[0] == 4 and version[1] >= 2) {
		return kIndexedFrag420;
	}
	return kIndexedFrag330;
}

static constexpr byte_t kLightingFrag420[] = R"({
#version 420 core
struct Light {
	vec4 internal;
	vec2 center;
	float depth, diameter;
	vec4 attenuate, color;
};
layout(binding = 0, std140) uniform transforms {
	mat4 drawmatrix;
	vec2 dimensions;
	vec2 resolution;
};
layout(binding = 1, std140) uniform lighting {
	uint total;
	Light lights[32];
};
layout(binding = 0) uniform sampler2DArray framebuf;
layout(location = 0) out vec4 fragment;
void main() {
	vec2 scoords = gl_FragCoord.xy / dimensions;
	vec4 diffuse = texture(framebuf, vec3(scoords, 0.0f));
	fragment = diffuse;
	for(uint i = 0U; i < total; ++i) {
		Light light = lights[i];
		vec4 currpos = drawmatrix * vec4(light.center, 0.0f, 1.0f) * 0.5f + 0.5f;
		vec3 currdir = vec3(currpos.xy - scoords, light.depth);
		currdir.xy /= (light.diameter / resolution);
		float D = length(currdir);
		float A = 1.0f / (light.attenuate.x + (D * light.attenuate.y) + (D * D * light.attenuate.z));
		vec3 lambert = (light.color.rgb * light.color.a);
		fragment.rgb += (diffuse.rgb * lambert * A);
	}
})";

static constexpr byte_t kLightingFrag330[] = R"({
#version 330 core
struct Light {
	vec4 internal;
	vec2 center;
	float depth, diameter;
	vec4 attenuate, color;
};
layout(std140) uniform transforms {
	mat4 drawmatrix;
	vec2 dimensions;
	vec2 resolution;
};
layout(std140) uniform lighting {
	uint total;
	Light lights[32];
};
uniform sampler2DArray framebuf;
layout(location = 0) out vec4 fragment;
void main() {
	vec2 scoords = gl_FragCoord.xy / dimensions;
	vec4 diffuse = texture(framebuf, vec3(scoords, 0.0f));
	fragment = diffuse;
	for(uint i = 0U; i < total; ++i) {
		Light light = lights[i];
		vec4 currpos = drawmatrix * vec4(light.center, 0.0f, 1.0f) * 0.5f + 0.5f;
		vec3 currdir = vec3(currpos.xy - scoords, light.depth);
		currdir.xy /= (light.diameter / resolution);
		float D = length(currdir);
		float A = 1.0f / (light.attenuate.x + (D * light.attenuate.y) + (D * D * light.attenuate.z));
		vec3 lambert = (light.color.rgb * light.color.a);
		fragment.rgb += (diffuse.rgb * lambert * A);
	}
})";

std::string pipeline::lighting_frag(glm::ivec2 version) {
	if (version[0] == 4 and version[1] >= 2) {
		return kLightingFrag420;
	}
	return kLightingFrag330;
}
