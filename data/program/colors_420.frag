#version 420 core

in STAGE {
	layout(location = 0) vec4 vtxcolor;
} fs;

layout(location = 0) out vec4 fragment;

void main() {
	fragment = fs.vtxcolor;
}