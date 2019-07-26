
#version 330

uniform sampler2D texture_map;

in vec3 out_vertex;
in vec2 st;

out vec4 fragColour;

void main(void)
{
		fragColour = texture(texture_map, st);
		// fragColour = vec4(out_vertex, 1.0);
}
