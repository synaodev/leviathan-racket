#version 420 core

layout(binding = 0) uniform sampler2DArray texturemap;

in STAGE {
	layout(location = 0) vec3 uvcoords;
	layout(location = 1) float vtxalpha;
} fs;

layout(location = 0) out vec4 diffuse;

void main() {
	vec4 color = texture(texturemap, vec3(fs.uvcoords.xy, 0.0f));
	diffuse = vec4(color.rgb, color.a * fs.vtxalpha);
}