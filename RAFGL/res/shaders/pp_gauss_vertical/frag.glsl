#version 330

in vec2 pass_uvs[11];

layout (location = 0) out vec4 final_colour;

uniform sampler2D tex;

void main()
{
	float coeffs[11];
	coeffs[0]  = 0.0093;
	coeffs[1]  = 0.028002;
	coeffs[2]  = 0.065984;
	coeffs[3]  = 0.121703;
	coeffs[4]  = 0.175713;
	coeffs[5]  = 0.198596;
	coeffs[6]  = 0.175713;
	coeffs[7]  = 0.121703;
	coeffs[8]  = 0.065984;
	coeffs[9]  = 0.028002;
	coeffs[10] = 0.0093;
	
	vec3 colour = vec3(0.0);
	for(int i = 0; i < 11; i++)
	{
		colour += texture(tex, pass_uvs[i]).rgb * coeffs[i];
	}

	final_colour = vec4(colour, 1.0);
}
