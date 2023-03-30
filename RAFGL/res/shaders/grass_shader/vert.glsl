#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 colour;
layout (location = 2) in float alpha;
layout (location = 3) in vec2 uv;
layout (location = 4) in vec3 normal;
layout (location = 5) in float random_number;

out vec3 pass_colour;
out vec2 pass_uv;
out vec3 pass_normal;
out float visibility;
out vec3 pass_position;
out float pass_alpha;

out vec3 pass_specular_colour;
out vec3 pass_diffuse_colour;

out vec3 pass_world_position;

const float density = 0.02;
const float gradient = 0.25;

uniform float uni_phase;
uniform vec3 uni_camera_pos;
uniform mat4 uni_M;
uniform mat4 uni_VP;




void main()
{

	vec3 sin_position = vec3(position.x + sin(uni_phase * random_number), position.y + sin(uni_phase * random_number)*2 + cos(uni_phase * random_number)*2, position.z);

	vec4 world_position = uni_M * vec4(sin_position, 1.0);
	
	vec4 position_relative_to_camera = uni_VP * world_position;

	pass_world_position = world_position.xyz;
	
	gl_Position = position_relative_to_camera;
	
	vec3 uni_light_direction = vec3(-1, -1, -1);
	vec3 uni_light_colour = vec3(0.2, 0.2, 0.2);
	vec3 uni_object_colour = vec3(0.01, 0.0, 0.0);
	vec3 uni_ambient = vec3(0.16, 0.16, 0.16);
	
	vec3 view_vector = normalize(world_position.xyz - uni_camera_pos);
	vec3 neg_view_vector = view_vector * (-1);
	vec3 normalized_normal = normalize((uni_M * vec4(normal, 0.0)).xyz);
	vec3 reflected_light = reflect(uni_light_direction, normalized_normal);
	
	float specular_factor = clamp(dot(reflected_light, neg_view_vector), 0, 1);
	specular_factor = pow(specular_factor, 5.0);
	pass_specular_colour = uni_light_colour * specular_factor;
	
	float light_factor = clamp(dot(normalize((uni_M * vec4(normal, 0.0)).xyz), normalize(uni_light_direction * (-1))), 0, 1);
    pass_diffuse_colour = (uni_ambient * uni_object_colour) + (uni_object_colour * (uni_light_colour * light_factor));
	
	
	
	
	
	
	pass_colour = vec3(sin_position.y / 50, sin_position.y / 50, sin_position.y / 50);
	pass_position = sin_position;
	//pass_colour = colour;
	pass_uv = uv;
	pass_normal = (uni_VP * vec4(normal, 0.0)).xyz;
	pass_alpha = alpha;
	
	
	
	
	
	float distance = length(position_relative_to_camera.xyz);
	visibility = clamp(exp(-pow((distance * density), gradient)), 0.0, 1.0);
}