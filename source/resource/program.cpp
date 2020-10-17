#include "./program.hpp"

#include "../video/gl-check.hpp"

#include <fmt/format.h>

static constexpr byte_t kMinorVert420[] = R"(#version 420 core
layout(location = 0) in vec2 position;
out STAGE {
	layout(location = 0) vec4 color;
} vs;
void main() {
	gl_Position = vec4(position, 0.0f, 1.0f);
	vs.color = vec4(1.0f);
})";

static constexpr byte_t kMinorVert330[] = R"(#version 330 core
layout(location = 0) in vec2 position;
out STAGE {
	vec4 color;
} vs;
void main() {
	gl_Position = vec4(position, 0.0f, 1.0f);
	vs.color = vec4(1.0f);
})";

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
layout(location = 2) in vec2 uvcoords;
layout(location = 3) in float alpha;
layout(location = 4) in int texID;
layout(location = 5) in int palID;
out STAGE {
	layout(location = 0) vec2 uvcoords;
	layout(location = 1) float alpha;
	layout(location = 2) flat int texID;
	layout(location = 3) flat int palID;
} vs;
void main() {
	gl_Position = viewports[matrix] * vec4(position, 0.0f, 1.0f);
	vs.uvcoords = uvcoords;
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
layout(location = 2) in vec2 uvcoords;
layout(location = 3) in float alpha;
layout(location = 4) in int texID;
layout(location = 5) in int palID;
out STAGE {
	vec2 uvcoords;
	float alpha;
	flat int texID;
	flat int palID;
} vs;
void main() {
	gl_Position = viewports[matrix] * vec4(position, 0.0f, 1.0f);
	vs.uvcoords = uvcoords;
	vs.alpha = alpha;
	vs.texID = texID;
	vs.palID = palID;
})";

static constexpr byte_t kFontsVert420[] = R"(#version 420 core
layout(std140) uniform transforms {
	mat4 viewports[2];
};
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uvcoords;
layout(location = 2) in vec4 color;
layout(location = 3) in int atlas;
layout(location = 4) in int table;
out STAGE {
	layout(location = 0) vec2 uvcoords;
	layout(location = 1) vec4 color;
	layout(location = 2) flat int atlas;
	layout(location = 3) flat int table;
} vs;
void main() {
	gl_Position = viewports[0] * vec4(position, 0.0f, 1.0f);
	vs.uvcoords = uvcoords;
	vs.color = color;
	vs.atlas = atlas;
	vs.table = table;
})";

static constexpr byte_t kFontsVert330[] = R"(#version 330 core
layout(std140) uniform transforms {
	mat4 viewports[2];
};
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uvcoords;
layout(location = 2) in vec4 color;
layout(location = 3) in int atlas;
layout(location = 4) in int table;
out STAGE {
	vec2 uvcoords;
	vec4 color;
	flat int atlas;
	flat int table;
} vs;
void main() {
	gl_Position = viewports[0] * vec4(position, 0.0f, 1.0f);
	vs.uvcoords = uvcoords;
	vs.color = color;
	vs.atlas = atlas;
	vs.table = table;
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
layout(binding = 0) uniform sampler2DArray diffuse;
in STAGE {
	layout(location = 0) vec2 uvcoords;
	layout(location = 1) float alpha;
	layout(location = 2) flat int texID;
	layout(location = 3) flat int palID;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 color = texture(diffuse, vec3(fs.uvcoords, float(fs.texID)));
	fragment = vec4(color.rgb, color.a * fs.alpha);
})";

static constexpr byte_t kSpritesFrag330[] = R"(#version 330 core
uniform sampler2DArray diffuse;
in STAGE {
	vec2 uvcoords;
	float alpha;
	flat int texID;
	flat int palID;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 color = texture(diffuse, vec3(fs.uvcoords, float(fs.texID)));
	fragment = vec4(color.rgb, color.a * fs.alpha);
})";

static constexpr byte_t kIndexedFrag420[] = R"(#version 420 core
layout(binding = 0) uniform sampler2DArray diffuse;
layout(binding = 1) uniform sampler1DArray palette;
in STAGE {
	layout(location = 0) vec2 uvcoords;
	layout(location = 1) float alpha;
	layout(location = 2) flat int texID;
	layout(location = 3) flat int palID;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 pixel = texture(diffuse, vec3(fs.uvcoords, float(fs.texID)));
	vec4 color = texture(palette, vec2(pixel[0], float(fs.palID)));
	fragment = vec4(color.rgb, color.a * fs.alpha);
})";

static constexpr byte_t kIndexedFrag330[] = R"(#version 330 core
uniform sampler2DArray diffuse;
uniform sampler1DArray palette;
in STAGE {
	vec2 uvcoords;
	float alpha;
	flat int texID;
	flat int palID;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 pixel = texture(diffuse, vec3(fs.uvcoords, float(fs.texID)));
	vec4 color = texture(palette, vec2(pixel[0], float(fs.palID)));
	fragment = vec4(color.rgb, color.a * fs.alpha);
})";

static constexpr byte_t kChannelsFrag420[] = R"(#version 420 core
layout(binding = 2) uniform sampler2DArray channels;
in STAGE {
	layout(location = 0) vec2 uvcoords;
	layout(location = 1) vec4 color;
	layout(location = 2) flat int atlas;
	layout(location = 3) flat int table;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 pixel = texture(channels, vec3(fs.uvcoords, float(fs.atlas)));
	vec4 color = vec4(1.0f - pixel[fs.table]);
	fragment = color * fs.color;
})";

static constexpr byte_t kChannelsFrag330[] = R"(#version 330 core
uniform sampler2DArray channels;
in STAGE {
	vec2 uvcoords;
	vec4 color;
	flat int atlas;
	flat int table;
} fs;
layout(location = 0) out vec4 fragment;
void main() {
	vec4 pixel = texture(channels, vec3(fs.uvcoords, float(fs.atlas)));
	vec4 color = vec4(1.0f - pixel[fs.table]);
	fragment = color * fs.color;
})";

namespace program {
	std::string directive() {
		return fmt::format(
			"#version {}{}0",
			opengl_version[0],
			opengl_version[1]
		);
	}
	std::string minor_vert() {
		if (opengl_version[0] == 4 and opengl_version[1] >= 2) {
			return kMinorVert420;
		}
		return kMinorVert330;
	}
	std::string blank_vert() {
		if (opengl_version[0] == 4 and opengl_version[1] >= 2) {
			return kBlankVert420;
		}
		return kBlankVert330;
	}
	std::string major_vert() {
		if (opengl_version[0] == 4 and opengl_version[1] >= 2) {
			return kMajorVert420;
		}
		return kMajorVert330;
	}
	std::string fonts_vert() {
		if (opengl_version[0] == 4 and opengl_version[1] >= 2) {
			return kFontsVert420;
		}
		return kFontsVert330;
	}
	std::string colors_frag() {
		if (opengl_version[0] == 4 and opengl_version[1] >= 2) {
			return kColorsFrag420;
		}
		return kColorsFrag330;
	}
	std::string sprites_frag() {
		if (opengl_version[0] == 4 and opengl_version[1] >= 2) {
			return kSpritesFrag420;
		}
		return kSpritesFrag330;
	}
	std::string indexed_frag() {
		if (opengl_version[0] == 4 and opengl_version[1] >= 2) {
			return kIndexedFrag420;
		}
		return kIndexedFrag330;
	}
	std::string channels_frag() {
		if (opengl_version[0] == 4 and opengl_version[1] >= 2) {
			return kChannelsFrag420;
		}
		return kChannelsFrag330;
	}
}
