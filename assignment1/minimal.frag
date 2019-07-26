
#version 330

#define PI 3.14159265359
// use vertex position - center location (transformation matrix) to
//  create centre base pattern that will be fixed if rotation matrix
//  purely handles movement of object
in vec3 out_vertex;
in float out_time;



// The final colour we will see at this location on screen
out vec4 fragColour;

void main(void)
{

  vec2 st = out_vertex.xy;
  st = fract(st);
  vec3 colour = vec3(st, 0);

  if ((st.x > 0.25) && (st.x < 0.75) && (st.y > 0.25) && (st.y < 0.75)) {
    colour = vec3(sin(2 * out_time + PI), 0.0, sin(2 * out_time));

  }
  else {
    colour = vec3(0.0, 1.0, 0.0);
  }

  // vec3 colour = vec3(st, 0);
	fragColour = vec4(colour, 1.0);
}
