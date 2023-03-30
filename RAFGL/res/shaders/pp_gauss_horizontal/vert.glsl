#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;


out vec2 pass_uvs[11];
out vec2 pass_uv;

uniform float target_width;

void main()
{

	gl_Position = vec4(position, 1.0);
	float pixel_size = 2.0 / target_width;
	for(int i = -5; i <= 5; i++)
	{
		pass_uvs[i + 5] = uv + vec2((i + 0.25) * pixel_size, 0.0);
	}
	
	pass_uv = uv;

}
