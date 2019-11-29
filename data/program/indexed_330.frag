#version 330 core

uniform sampler2DArray indexedmap;
uniform sampler1DArray palettemap;

in STAGE {
	layout(location = 0) vec3 uvcoords;
	layout(location = 1) float vtxalpha;
} fs;

layout(location = 0) out vec4 diffuse;

void main() {
	vec4 index = texture(indexedmap, vec3(fs.uvcoords.xy, 0.0f));
	vec4 color = texture(palettemap, vec2(index[0], fs.uvcoords.z));
	diffuse = vec4(color.rgb, color.a * fs.vtxalpha);
}