#version 430 core

in layout(location = 0) vec3 position;
in layout(location = 1) vec2 texcoord;
in layout(location = 2) vec3 blendMap;

uniform layout(location = 0) mat4 MVP;

out layout(location = 0) vec2 texcoord_out;
out layout(location = 1) vec3 blendMap_out;

void main()
{
	texcoord_out = texcoord;
	blendMap_out = blendMap;
    gl_Position = MVP*vec4(position, 1.0f);
}
