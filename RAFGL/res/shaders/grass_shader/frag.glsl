#version 330

in vec3 pass_colour;
in vec2 pass_uv;
in vec3 pass_normal;
in float pass_alpha;

in vec3 pass_world_position;

in float visibility;

in vec3 pass_position;

in vec3 pass_specular_colour;
in vec3 pass_diffuse_colour;

out vec4 final_colour;

uniform sampler2D normal_map;

uniform float uni_phase;
uniform vec3 uni_camera_pos;
uniform vec3 sky_colour;




void main()
{
	//final_colour = vec4(1.0, 1.0, 1.0, 1.0);
	vec3 texture_colour = texture(normal_map, pass_uv).rgb;
	
	
	
	final_colour = vec4(texture_colour + pass_diffuse_colour+ pass_colour, 1.0);
	
	
	//vec3 new_colour = vec3(pass_colour.x, pass_colour.y, pass_colour.z);
	
	//final_colour = vec4(new_colour, 1.0);
	
	/*
	vec3 normalized_normal = normalize(pass_normal);
	vec2 uv_coord = pass_world_position.xz;
	
	vec2 uv_coord1 = vec2(uv_coord.x , uv_coord.y );
    vec2 uv_coord2 = vec2(uv_coord.x, uv_coord.y );
    vec2 uv_coord3 = vec2(uv_coord.x, uv_coord.y );
	
	vec3 accum1 = texture(normal_map, uv_coord1).rgb;
    vec3 accum2 = texture(normal_map, uv_coord2).rgb;
    vec3 accum3 = texture(normal_map, uv_coord3).rgb;
    vec3 total = normalize((accum1 + accum2 + accum3));
	
	float sky_colour_factor = total.y;
    sky_colour_factor = clamp(sky_colour_factor, 1.0, 1.0);
	
	vec3 to_camera_vec = normalize(pass_world_position - uni_camera_pos);
	
	float specular_factor = dot(reflect(normalize(vec3(1.0, 1.0, 1.0)), total.xyz), to_camera_vec);
	specular_factor = clamp(specular_factor, 0.0, 1.0);
    specular_factor = pow(specular_factor, 20.0);

    vec3 diffuse_color = mix(vec3(0.02, 0.02, 0.5), vec3(0.4, 0.6, 0.8), sky_colour_factor);

    final_colour = vec4(diffuse_color, 1.0);
    final_colour.a = 1.0;
	*/

	
}