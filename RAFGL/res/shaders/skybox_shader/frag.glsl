#version 330

layout (location = 0) out vec4 final_colour;
layout (location = 1) out vec4 bright;
layout (location = 2) out vec4 final_normal;

in vec3 tex_coords;
in vec3 pass_normal;

uniform samplerCube skybox;

float brightness(vec3 pix)
{
	return (pix.r + pix.g + pix.b) / 3.0;
}

void main()
{    
    final_colour = texture(skybox, tex_coords);
	bright *= bright;
	bright = clamp(final_colour - 0.75, vec4(0), vec4(1)) * 4;
	
	final_normal = vec4((normalize(pass_normal) + 1.0) / 2.0, 1.0);
}