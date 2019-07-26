
#version 330


uniform sampler2D texture_map;
uniform bool texture_toggle;

in vec3 out_vertex;
in vec2 st;
in vec4 out_normal;
in vec4 out_light_position;

// out vec4 fragColour;

void main(void)
{
		vec3 ambient = vec3(1.0)*0.10;

		float x = length(vec2(320) - gl_FragCoord.xy)/640;
		float torch_factor = pow(cos(x*3.14), 5);
		if (x > 0.5) {
			torch_factor = 0;
		}

		// torch profile shaping
		float intensity = 25*log(1/gl_FragCoord.z);
		
		// reflection on basis of normal
		vec3 light = out_light_position.xyz;
		float reflect = 0.75*abs(dot(normalize(light), -1*normalize(out_normal.xyz)));
		
		// diffuse + specular handling
		float diffuse_specular = torch_factor*intensity * reflect;

		vec4 colour = texture(texture_map,st)*float(texture_toggle) + vec4(1, 1, 1, 1.0)*float(!texture_toggle);
		gl_FragColor = vec4(colour.x * (ambient.x + diffuse_specular),
												colour.y * (ambient.y + diffuse_specular),
												colour.z * (ambient.z + diffuse_specular),
												1.0);
}
