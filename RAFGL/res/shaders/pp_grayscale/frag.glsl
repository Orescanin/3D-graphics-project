#version 330

in vec2 pass_uv;

layout (location = 0) out vec4 final_colour;

uniform sampler2D tex;

void main()
{
	
	vec3 colour = texture(tex, pass_uv).rgb;
	float brightness = (0.2125 * colour.r) + (0.7154 * colour.g) + (0.0721 * colour.b);
	final_colour = vec4(vec3(brightness), 1.0);

}
