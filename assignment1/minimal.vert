
#version 330

// Note this vertex shader executes once per vertex in the array
layout (location = 0) in vec3 in_vertex;

uniform mat4 model;
uniform float time;

out vec3 out_vertex;
out float out_time;

void main(void)
{
  // gl_position is a built in variable to store vertex position
  // It has 4 coordinates for reasons we'll see later on.
  out_time = time;
  out_vertex = in_vertex;
	gl_Position = model * vec4(in_vertex, 1.0);
}
