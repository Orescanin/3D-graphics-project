#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;


out vec2 pass_uvs[11];

uniform float target_height;

void main()
{

	gl_Position = vec4(position, 1.0);
	float pixel_size = 2.0 / target_height;
	for(int i = -5; i <= 5; i++)
	{
		pass_uvs[i + 5] = uv + vec2(0.0, (i + 0.25) * pixel_size);
	}

}
