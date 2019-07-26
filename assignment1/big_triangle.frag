
#version 330

// pattern based in fragment location
// smooth
in vec3 out_vertex;

// colour output
out vec4 fragColour;

void main(void)
{
		vec2 st = out_vertex.xy;

		// produces 8x8 overlay
		// each square 0.125 by 0.125
		st = fract(4*st);

		// set default to black, then create circles
		vec3 colour = vec3(0, 0, 0);

		// define circle with in centre of division
		if (distance(st,vec2(0.5,0.5)) <= 0.5*0.5) {
			colour = vec3(0, 1, 1);
		}


		if (distance(st,vec2(0,0)) <= 0.5*0.5) {
			colour = vec3(0, 1, 1);
		}
		if (distance(st,vec2(1,1)) <= 0.5*0.5) {
			colour = vec3(0, 1, 1);
		}
		if (distance(st,vec2(1,0)) <= 0.5*0.5) {
			colour = vec3(0, 1, 1);
		}
		if (distance(st,vec2(0,1)) <= 0.5*0.5) {
			colour = vec3(0, 1, 1);
		}


		fragColour = vec4(colour, 1);

}
