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
}