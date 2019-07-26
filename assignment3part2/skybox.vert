
#version 330

layout (location = 0) in vec3 a_vertex;

uniform mat4 projection;
uniform mat4 view;

out vec3 texture_coords;

void main(void)
{
	texture_coords = a_vertex;
	gl_Position = projection * view * vec4(a_vertex, 1.0);
}
