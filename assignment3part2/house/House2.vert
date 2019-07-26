
#version 330

layout (location = 0) in vec3 a_vertex;
layout (location = 2) in vec3 a_normal;
layout (location = 3) in vec2 a_texcoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec4 pos;
out vec3 normal;
out vec2 tex_coord;

void main(void)
{
	// lighting to be done in eye space
	pos = view * model * vec4(a_vertex, 1.0);
	normal = vec3(normalize(transpose(inverse(view * model)) * vec4(a_normal, 1.0)));
	tex_coord = a_texcoord;
	gl_Position = projection * view * model * vec4(a_vertex, 1.0);
}
