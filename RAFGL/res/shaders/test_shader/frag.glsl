#version 330

in vec3 pass_colour;
in vec2 pass_uv;
in vec3 pass_normal;

in vec3 pass_world_position;

out vec4 final_colour;

uniform sampler2D text;

uniform float uni_phase;
uniform vec3 uni_camera_pos;


void main()
{
	
	final_colour = texture(text, pass_uv);


	
}