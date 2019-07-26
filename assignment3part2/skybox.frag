
#version 330

uniform samplerCube skybox;

in vec3 texture_coords;

out vec4 fragColour;

void main(void) {

	fragColour = texture(skybox, texture_coords);
}
