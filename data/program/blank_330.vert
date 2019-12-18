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
}