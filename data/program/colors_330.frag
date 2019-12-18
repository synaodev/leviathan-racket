#version 330 core

in STAGE {
	vec4 color;
} fs;

layout(location = 0) out vec4 fragment;

void main() {
	fragment = fs.color;
}