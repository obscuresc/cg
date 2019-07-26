
#version 330

uniform sampler2D texture_map;
uniform int light_mode;
uniform vec4 view_position;
uniform float time;


in vec4 pos;
in vec2 tex_coord;
in vec3 normal;

out vec4 fragColour;

void main(void)
{
	// lighting disabled
	if (light_mode == 0) {

		fragColour = texture(texture_map, tex_coord);
	}

	// top down red diffuse + blue specular directional lighting
	// lighting is performed in eye space
	else if (light_mode == 1) {

		vec3 light_colour_red = vec3(1.0f, 0.5f, 0.5f);
		vec3 light_colour_blue = vec3(0.5f, 0.5f, 1.0f);

    // ambient
    float ambient_scale = 0.25f;
    vec3 ambient = ambient_scale * vec3(1);

    // diffuse
    vec3 light_position = vec3(0.0f, 2.0f, -1.0f);
		vec3 light_direction = normalize(vec3(0.0f, 1.0f, 0.0f));
    float diff = 2 * max(dot(normal, light_direction), 0.0f);
    vec3 diffuse = diff * light_colour_red;

    // specular
		// testing specular not quite right and needs shininess parameter
    vec3 view_direction = normalize(view_position.xyz - pos.xyz);
		vec3 reflect_direction = reflect(-light_position, normal.xyz);
		vec3 halfway_direction = normalize(light_position + view_direction);
		float testing_shininess = 2;
		float spec = pow(max(dot(normal, halfway_direction), 0.0), testing_shininess);
    float specular_scale = 10;
		vec3 specular = specular_scale * spec * light_colour_blue;

    vec3 result = (ambient + specular) * texture(texture_map, tex_coord).xyz;
    fragColour = vec4(result, 1.0);
	}

	// green head lighting
	else if (light_mode == 2) {

		vec3 light_colour_green = vec3(0.5f, 1.0f, 0.5f);

		// ambient
		float ambient_scale = 0.25;
		vec3 ambient = ambient_scale * vec3(1);

		// diffuse
		vec3 light_position = vec3(0.0, 0.0, 0.0);
		vec3 light_direction = normalize(light_position - pos.xyz);
		float diff = 2 * max(dot(normal, light_direction), 0.0);
		vec3 diffuse = diff * light_colour_green;

		// blinn phong specular
		vec3 view_direction = normalize(view_position.xyz - pos.xyz);
		vec3 reflect_direction = reflect(-light_direction, normal.xyz);
		vec3 halfway_direction = normalize(light_direction + view_direction);
		float testing_shininess = 10;
		float spec = pow(max(dot(normal, halfway_direction), 0.0), testing_shininess);
		float specular_scale = 5;
		vec3 specular = specular_scale * spec * light_colour_green;

		vec3 result = (ambient + diffuse + specular) * texture(texture_map, tex_coord).xyz;
		fragColour = vec4(result, 1.0);
	}

	// rotational yellow light
	else {

		vec3 light_colour_yellow = vec3(1.0f, 1.0f, 0.5f);

		// ambient
		float ambient_scale = 0.25;
		vec3 ambient = ambient_scale * vec3(1);

		// diffuse
		vec3 light_position = vec3(0.0, 2.0, -2.0);
		light_position = mat3(cos(time), 0, sin(time),
													0, 1, 0,
													-1*(sin(time)), 0, cos(time)) * light_position;
		vec3 light_direction = normalize(light_position - pos.xyz);
		float diff = 2 * max(dot(normal, light_direction), 0.0);
		vec3 diffuse = diff * light_colour_yellow;

		// blinn phong
		// specular
		vec3 view_direction = normalize(view_position.xyz - pos.xyz);
		vec3 reflect_direction = reflect(-light_direction, normal.xyz);
		vec3 halfway_direction = normalize(light_direction + view_direction);
		float testing_shininess = 10;
		float spec = pow(max(dot(normal, halfway_direction), 0.0), testing_shininess);
		float specular_scale = 5;
		vec3 specular = specular_scale * spec * light_colour_yellow;

		vec3 result = (ambient + diffuse + specular) * texture(texture_map, tex_coord).xyz;
		fragColour = vec4(result, 1.0);
	}
}
