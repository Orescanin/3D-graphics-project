#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;


out vec2 pass_uv;
uniform mat4 uni_M;
uniform mat4 uni_VP;

void main()
{

	gl_Position = uni_VP * uni_M * vec4(position, 1.0);
	pass_uv = uv;
	pass_uv.y = 1.0 - pass_uv.y;
	
}
