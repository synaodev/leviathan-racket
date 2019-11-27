#version 420 core

struct Light {
	vec4 internal;
	vec2 center;
	float depth, diameter;
	vec4 attenuate, color;
};

layout(binding = 0, std140) uniform transforms {
	mat4 drawmatrix;
	vec2 dimensions;
	vec2 resolution;
};

layout(binding = 1, std140) uniform lighting {
	uint total;
	Light lights[32];
};

layout(binding = 0) uniform sampler2DArray framebuf;
layout(location = 0) out vec4 fragment;

void main() {
	vec2 scoords = gl_FragCoord.xy / dimensions;
	vec4 diffuse = texture(framebuf, vec3(scoords, 0.0f));
	fragment = diffuse;
	for(uint i = 0U; i < total; ++i) {
		Light light = lights[i];
		vec4 currpos = drawmatrix * vec4(light.center, 0.0f, 1.0f) * 0.5f + 0.5f;
		vec3 currdir = vec3(currpos.xy - scoords, light.depth);
		currdir.xy /= (light.diameter / resolution);
		float D = length(currdir);
		float A = 1.0f / (light.attenuate.x + (D * light.attenuate.y) + (D * D * light.attenuate.z));
		vec3 lambert = (light.color.rgb * light.color.a);
		fragment.rgb += (diffuse.rgb * lambert * A);
	}
}