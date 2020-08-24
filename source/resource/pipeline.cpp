#include "./pipeline.hpp"

static constexpr byte_t kMinorVert420[] = R"(#version 420 core
layout(location = 0) in vec2 position;
void main() {
	gl_Position = vec4(position, 0.0f, 1.0f);
})";

static constexpr byte_t kMinorVert330[] = R"(#version 330 core
layout(location = 0) in vec2 position;
void main() {
	gl_Position = vec4(position, 0.0f, 1.0f);
})";

static constexpr byte_t kBlankVert420[] = R"(#version 420 core
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

static constexpr byte_t kBlankVert330[] = R"(#version 330 core
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

static constexpr byte_t kMajorVert420[] = R"(#version 420 core
layout(binding = 0, std140) uniform transforms {
	mat4 viewport;
	vec2 dimensions;
	vec2 resolution;
};
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in float alpha;
out STAGE {
	layout(location = 0) vec3 uvs;
	layout(location = 1) float alpha;
} vs;
void main() {
	gl_Position = viewport * vec4(position, 0.0f, 1.0f);
	vs.uvs = uvs;
	vs.alpha = alpha;
})";

static constexpr byte_t kMajorVert330[] = R"(#version 330 core
layout(std140) uniform transforms {
	mat4 viewport;
	vec2 dimensions;
	vec2 resolution;
};
layout(location = 0) in vec2 position;
layout(location = 1) in vec3 uvs;
layout(location = 2) in float alpha;
out STAGE {
	vec3 uvs;
	float alpha;
} vs;
void main() {
	gl_Position = viewport * vec4(position, 0.0f, 1.0f);
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

static constexpr byte_t kLightingFrag420[] = R"(#version 420 core
#define MAXIMUM_LIGHTS 32
struct Light {
	vec4 internal;
	vec2 center;
	float depth, diameter;
	vec4 attenuate, color;
};
layout(binding = 0, std140) uniform transforms {
	mat4 viewport;
	vec2 dimensions;
	vec2 resolution;
};
layout(binding = 1, std140) uniform lighting {
	uint total;
	Light lights[MAXIMUM_LIGHTS];
};
layout(binding = 0) uniform sampler2DArray framebuf;
layout(location = 0) out vec4 fragment;
void main() {
	vec2 scoords = gl_FragCoord.xy / dimensions;
	vec4 diffuse = texture(framebuf, vec3(scoords, 0.0f));
	fragment = diffuse;
	for(uint i = 0U; i < total; ++i) {
		vec4 currpos = viewport * vec4(lights[i].center, 0.0f, 1.0f) * 0.5f + 0.5f;
		vec3 currdir = vec3(currpos.xy - scoords, lights[i].depth);
		currdir.xy /= (lights[i].diameter / resolution);
		float D = length(currdir);
		float A = 1.0f / (lights[i].attenuate.x + (D * lights[i].attenuate.y) + (D * D * lights[i].attenuate.z));
		vec3 lambert = (lights[i].color.rgb * lights[i].color.a);
		fragment.rgb += (diffuse.rgb * lambert * A);
	}
})";

static constexpr byte_t kLightingFrag330[] = R"(#version 330 core
#define MAXIMUM_LIGHTS 32
struct Light {
	vec4 internal;
	vec2 center;
	float depth, diameter;
	vec4 attenuate, color;
};
layout(std140) uniform transforms {
	mat4 viewport;
	vec2 dimensions;
	vec2 resolution;
};
layout(std140) uniform lighting {
	uint total;
	Light lights[MAXIMUM_LIGHTS];
};
uniform sampler2DArray framebuf;
layout(location = 0) out vec4 fragment;
void main() {
	vec2 scoords = gl_FragCoord.xy / dimensions;
	vec4 diffuse = texture(framebuf, vec3(scoords, 0.0f));
	fragment = diffuse;
	for(uint i = 0U; i < total; ++i) {
		vec4 currpos = viewport * vec4(lights[i].center, 0.0f, 1.0f) * 0.5f + 0.5f;
		vec3 currdir = vec3(currpos.xy - scoords, lights[i].depth);
		currdir.xy /= (lights[i].diameter / resolution);
		float D = length(currdir);
		float A = 1.0f / (lights[i].attenuate.x + (D * lights[i].attenuate.y) + (D * D * lights[i].attenuate.z));
		vec3 lambert = (lights[i].color.rgb * lights[i].color.a);
		fragment.rgb += (diffuse.rgb * lambert * A);
	}
})";

namespace pipeline {
	std::string minor_vert(glm::ivec2 version) {
		if (version[0] == 4 and version[1] >= 2) {
			return kMinorVert420;
		}
		return kMinorVert330;
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
	std::string lighting_frag(glm::ivec2 version) {
		if (version[0] == 4 and version[1] >= 2) {
			return kLightingFrag420;
		}
		return kLightingFrag330;
	}
}
