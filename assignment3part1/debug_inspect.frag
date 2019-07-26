
#version 330

uniform int debug_mode;
uniform vec3 diffuse_colour;

in vec3 normal;

out vec4 fragColour;

void main(void) {

	if (debug_mode == 0) {

		fragColour = vec4(1.0, 0.2, 0.2, 1.0);
	}
	else if (debug_mode == 1) {
    //
		// // red green, orange blue, yellow purple
		// float scale = 0.25;
		// vec3 colour = scale * vec3(1.0, 0.0, 0.0)*normal.x + vec3(0.0, 1.0, 0.0)*(1 - normal.x)+
		// scale * vec3(1.0, 0.5, 0.0)*(1 - normal.y) + vec3(0.0, 0.0, 1.0)*normal.y+
		// scale * vec3(1.0, 1.0, 0.0)*normal.z + vec3(0.5, 0.0, 1.0)*(1 - normal.z);
		// fragColour = vec4(colour, 1.0);

		fragColour = vec4(normal, 1.0);
	}
	else {

		fragColour = vec4(diffuse_colour, 1.0);
	}
}
