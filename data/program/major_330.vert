#version 330 core

layout(std140) uniform transforms {
	mat4 drawmatrix;
	vec2 dimensions;
	vec2 resolution;
};

layout(location = 0) in vec2 position;
layout(location = 1) in vec3 uvcoords;
layout(location = 2) in float vtxalpha;

out STAGE {
	layout(location = 0) vec3 uvcoords;
	layout(location = 1) float vtxalpha;
} vs;

void main() {
	gl_Position = drawmatrix * vec4(position, 0.0f, 1.0f);
	vs.uvcoords = uvcoords;
	vs.vtxalpha = vtxalpha;
}