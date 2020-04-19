#version 430 core

in layout(location = 0) vec2 texcoord;
in layout(location = 1) vec3 blendMap;
flat in layout(location = 2) uint noise_selector;
in layout(location = 3) vec3 normal;
in layout(location = 4) vec3 frag_pos;

//layout(binding = 0) uniform sampler2D plaintextureSampler;
//layout(binding = 1) uniform sampler2D mountaintextureSampler;
//layout(binding = 2) uniform sampler2D snowtextureSampler;
layout(binding = 0) uniform sampler2D noisetextureSampler1;
layout(binding = 1) uniform sampler2D noisetextureSampler2;
layout(binding = 2) uniform sampler2D noisetextureSampler3;

uniform layout(location=1) vec3 view_pos;

struct Light {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

out vec4 color;


void main()
{
	vec3 norm_blendMap = blendMap;
	vec3 norm_normal = normalize(normal);
    vec3 plaintex = vec3(0.0f, 0.4f, 0.0f);
	vec3 mountaintex = vec3(0.45f, 0.45f, 0.45f);
	vec3 snowtex = vec3(1.0f, 1.0f, 1.0f);
	
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm_normal, lightDir), 0.0);
	vec3 diffuse = diff * light.diffuse;
	
	vec3 viewDir = normalize(view_pos - frag_pos);
	vec3 reflectDir = reflect(-lightDir, norm_normal); 
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2); 	
	vec3 specular = spec * light.specular;
	
	if(noise_selector == 0) {
		vec3 noisetex = vec3(texture(noisetextureSampler1, texcoord));
		vec3 objectcolor = noisetex*((plaintex*norm_blendMap.x) + (mountaintex*norm_blendMap.y) + (snowtex*norm_blendMap.z));
		color = vec4(objectcolor*(light.ambient + diffuse + specular), 1.0f);
		//color = vec4(objectcolor, 1.0f);
		//color = vec4(noisetex, 1.0f);
	}
	else if(noise_selector == 1) {
		vec3 noisetex = vec3(texture(noisetextureSampler2, texcoord));
		vec3 objectcolor = noisetex*((plaintex*norm_blendMap.x) + (mountaintex*norm_blendMap.y) + (snowtex*norm_blendMap.z));
		color = vec4(objectcolor*(light.ambient + diffuse + specular), 1.0f);
		//color = vec4(objectcolor, 1.0f);
		//color = vec4(noisetex, 1.0f);
	}
	else {
		vec3 noisetex = vec3(texture(noisetextureSampler3, texcoord));
		vec3 objectcolor = noisetex*((plaintex*norm_blendMap.x) + (mountaintex*norm_blendMap.y) + (snowtex*norm_blendMap.z));
		color = vec4(objectcolor*(light.ambient + diffuse + specular), 1.0f);
		//color = vec4(objectcolor, 1.0f);
		//color = vec4(noisetex, 1.0f);
	}
}
