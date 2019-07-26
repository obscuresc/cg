
#version 330

// Note this vertex shader executes once per vertex in the array
layout (location = 0) in vec3 in_vertex;
layout (location = 1) in vec2 in_texture_coords;

// uniform float time;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;


out vec3 out_vertex;
out vec2 st;

void main(void) {

  out_vertex = in_vertex;
  st = in_texture_coords;
	gl_Position = projection * view * model * vec4(in_vertex, 1.0);
}
