#version 330 core

uniform sampler2D diffuse_map;

in STAGE {
	vec3 uvcoords;
	float alpha;
} fs;

layout(location = 0) out vec4 fragment;

void main() {
	vec4 color = texture(diffuse_map, fs.uvcoords.xy);
	fragment = vec4(color.rgb, color.a * fs.alpha);
}