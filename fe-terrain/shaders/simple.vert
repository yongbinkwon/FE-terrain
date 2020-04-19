#version 430 core

in layout(location = 0) vec3 position;
in layout(location = 1) vec2 texcoord;
in layout(location = 2) vec3 blendMap;
in layout(location = 3) vec3 normal;
in layout(location = 4) uint noise_selector;

uniform layout(location = 0) mat4 MVP;
uniform layout(location = 2) mat4 model;

out layout(location = 0) vec2 texcoord_out;
out layout(location = 1) vec3 blendMap_out;
flat out layout(location = 2) uint noise_selector_out;
out layout(location = 3) vec3 normal_out;
out layout(location = 4) vec3 frag_pos;

void main()
{
	texcoord_out = texcoord;
	frag_pos = vec3(model*vec4(position, 1.0f));
	noise_selector_out = noise_selector;
	blendMap_out = blendMap;
	normal_out = normalize(mat3(transpose(inverse(model)))*normal);
    gl_Position = MVP*vec4(position, 1.0f);
}
