#version 330

in vec3 pass_normal;
in vec3 pass_world_position;

layout (location = 0) out vec4 final_colour;
layout (location = 1) out vec4 bright;
layout (location = 2) out vec4 object_normal;


uniform vec3 uni_object_colour;
uniform vec3 uni_light_colour;
uniform vec3 uni_light_direction;
uniform vec3 uni_ambient;
uniform vec3 uni_camera_position;

float brightness(vec3 pix)
{
	return (pix.r + pix.g + pix.b) / 3.0;
}

void main()
{
	vec3 view_vector = normalize(pass_world_position - uni_camera_position);
	vec3 neg_view_vector = view_vector * (-1);
	vec3 normalized_normal = normalize(pass_normal);
	vec3 reflected_light = reflect(normalize(uni_light_direction), normalized_normal);
	
	float specular_factor = clamp(dot(reflected_light, neg_view_vector), 0, 1);
	specular_factor = pow(specular_factor, 5.0);
	vec3 specular_colour = uni_object_colour * uni_light_colour * specular_factor;
	
	float light_factor = clamp(dot(normalize(pass_normal), normalize(-uni_light_direction)), 0, 1);
    vec3 diffuse_colour = (uni_ambient * uni_object_colour) + (uni_object_colour * (uni_light_colour * light_factor));
	
	final_colour = vec4(diffuse_colour + specular_colour, 1.0);
	
	bright = clamp(final_colour - 0.5, vec4(0), vec4(1)) * 2;
	bright *= bright;
	bright = vec4(mix(bright.rgb, vec3(brightness(bright.rgb)), 0.75), 1.0);
	
	object_normal = vec4((normalized_normal + 1) / 2, 1.0);
	
}