
#version 330

layout (location = 0) in vec3 a_vertex;
layout (location = 2) in vec3 a_normal;
layout (location = 3) in vec2 a_texcoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 normal;

void main(void)
{
	normal = a_normal;
	gl_Position = projection * view * model * vec4(a_vertex, 1.0);
}
