#version 430 core

in layout(location = 0) vec2 texcoord;
in layout(location = 1) vec3 blendMap;
flat in layout(location = 2) uint noise_selector;

//layout(binding = 0) uniform sampler2D plaintextureSampler;
//layout(binding = 1) uniform sampler2D mountaintextureSampler;
//layout(binding = 2) uniform sampler2D snowtextureSampler;
layout(binding = 0) uniform sampler2D noisetextureSampler1;
layout(binding = 1) uniform sampler2D noisetextureSampler2;
layout(binding = 2) uniform sampler2D noisetextureSampler3;

out vec4 color;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 
uniform Material material;

void main()
{
	vec3 norm_blendMap = blendMap;
    vec3 plaintex = vec3(0.0f, 0.4f, 0.0f);
	vec3 mountaintex = vec3(0.5f, 0.5f, 0.5f);
	vec3 snowtex = vec3(1.0f, 1.0f, 1.0f);
	if(noise_selector == 0) {
		vec3 noisetex = vec3(texture(noisetextureSampler1, texcoord));
		color = vec4(noisetex*((plaintex*norm_blendMap.x) + (mountaintex*norm_blendMap.y) + (snowtex*norm_blendMap.z)), 1.0f);
		//color = vec4(noisetex, 1.0f);
	}
	else if(noise_selector == 1) {
		vec3 noisetex = vec3(texture(noisetextureSampler2, texcoord));
		color = vec4(noisetex*((plaintex*norm_blendMap.x) + (mountaintex*norm_blendMap.y) + (snowtex*norm_blendMap.z)), 1.0f);
		//color = vec4(noisetex, 1.0f);
	}
	else {
		vec3 noisetex = vec3(texture(noisetextureSampler3, texcoord));
		color = vec4(noisetex*((plaintex*norm_blendMap.x) + (mountaintex*norm_blendMap.y) + (snowtex*norm_blendMap.z)), 1.0f);
		//color = vec4(noisetex, 1.0f);
	}
}
