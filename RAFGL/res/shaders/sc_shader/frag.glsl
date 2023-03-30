#version 330

in vec2 pass_uv;

layout (location = 0) out vec4 final_colour;

uniform sampler2D tex;

void main()
{
	
	vec3 colour = texture(tex, pass_uv).rgb;
	final_colour = vec4(colour, 1.0);

}
