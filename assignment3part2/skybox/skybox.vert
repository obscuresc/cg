
#version 330

layout (location = 0) in vec3 a_vertex;

uniform mat4 projection;
uniform mat4 view;

out vec3 tex_coord;

void main(void)
{
	tex_coord = a_vertex;
	vec4 vertex = projection * view * vec4(a_vertex, 1.0);
	// gl_Position = projection * view * vec4(a_vertex, 1.0);
	gl_Position = vertex.xyww;
}
