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
	layout(location = 0) vec3 uvcoords;
	layout(location = 1) float alpha;
} vs;

void main() {
	gl_Position = viewport * vec4(position, 0.0f, 1.0f);
	vs.uvcoords = uvcoords;
	vs.alpha = alpha;
}