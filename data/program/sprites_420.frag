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
}