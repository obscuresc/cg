
#version 330

uniform samplerCube texture_map;

in vec3 tex_coord;

out vec4 fragColour;

void main(void)
{
	fragColour = texture(texture_map, tex_coord);
}
