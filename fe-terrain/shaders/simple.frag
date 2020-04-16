#version 430 core

in layout(location = 0) vec2 texcoord;
in layout(location = 1) vec3 blendMap;
in layout(location = 2) vec2 noisetexcoord;

layout(binding = 0) uniform sampler2D plaintextureSampler;
layout(binding = 1) uniform sampler2D mountaintextureSampler;
layout(binding = 2) uniform sampler2D snowtextureSampler;
layout(binding = 3) uniform sampler2D noisetextureSampler;

out vec4 color;

void main()
{
	vec3 norm_blendMap = blendMap;
    vec3 plaintex = vec3(texture(plaintextureSampler, texcoord));
	vec3 mountaintex = vec3(texture(mountaintextureSampler, texcoord));
	vec3 snowtex = vec3(texture(snowtextureSampler, texcoord));
	vec3 noisetex = vec3(texture(noisetextureSampler, noisetexcoord));
	color = vec4(noisetex*((plaintex*norm_blendMap.x) + (mountaintex*norm_blendMap.y) + (snowtex*norm_blendMap.z)), 1.0f);
	//color = vec4(noisetex, 1.0f);
}
