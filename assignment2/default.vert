
#version 330

// Note this vertex shader executes once per vertex in the array
layout (location = 0) in vec3 in_vertex;
layout (location = 1) in vec2 in_texture_coords;
layout (location = 3) in vec3 in_normal;

// uniform float time;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_position;

// value pass throughs
out vec3 out_vertex;
out vec2 st;
out vec4 out_normal;
out vec4 out_light_position;

void main(void) {

  out_vertex = in_vertex;
  st = in_texture_coords;
  out_normal = view * model * vec4(in_normal, 1.0);
	gl_Position = projection * view * model * vec4(in_vertex, 1.0);
	out_light_position = view * model * vec4(light_position-in_vertex, 1.0);
}
