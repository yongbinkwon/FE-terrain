#version 430 core

in layout(location = 0) vec2 texcoord;
in layout(location = 1) vec3 blendMap;

layout(binding = 0) uniform sampler2D plaintextureSampler;
layout(binding = 1) uniform sampler2D mountaintextureSampler;
layout(binding = 2) uniform sampler2D snowtextureSampler;

out vec4 color;

void main()
{
	vec3 norm_blendMap = blendMap;
    vec4 plaintex = texture(plaintextureSampler, texcoord);
	vec4 mountaintex = texture(mountaintextureSampler, texcoord);
	vec4 snowtex = texture(snowtextureSampler, texcoord);
	color = vec4((vec3(plaintex)*norm_blendMap.x) + (vec3(mountaintex)*norm_blendMap.y) + (vec3(snowtex)*norm_blendMap.z), 1.0f);
	//color = snowtex;
}
