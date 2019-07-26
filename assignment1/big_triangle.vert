
#version 330

// Note this vertex shader executes once per vertex in the array
layout (location = 0) in vec3 in_vertex;

uniform mat4 model;

out vec3 out_vertex;

void main(void)
{

  out_vertex = in_vertex;
	gl_Position = model * vec4(in_vertex, 1.0);
}
