#version 430

uniform sampler2D depth_tex; // texture uniform
uniform sampler2D colour_tex; // texture uniform

uniform int img_width;
uniform int img_height;
uniform float model_distance;
uniform float near;
uniform float far;




vec2 img_size = vec2(img_width, img_height);

in vec2 ftexcoord;

layout(location = 0) out vec4 frag_colour;

float to_distance(float depth_colour)
{
    float dist = (2.0*near*far) / (far + near - depth_colour*(far - near));	
    return dist;
}

float to_depth(float dist)
{
    float depth = (far*(dist - 2.0*near) + near*dist)/(dist*(far - near));
    return depth;
}

// https://www.shadertoy.com/view/Xltfzj
void main()
{
    const float pi_times_2 = 6.28318530718; // Pi*2
    
    float directions = 16.0; // BLUR directions (Default 16.0 - More is better but slower)
    float quality = 10.0; // BLUR quality (Default 4.0 - More is better but slower)
    float size = 8.0; // BLUR size (radius)

	vec4 unblurred_colour = texture(colour_tex, ftexcoord);
	float depth_colour = texture(depth_tex, ftexcoord).r;
 
    float distance_to_pixel = to_distance(depth_colour);
    float x = clamp(abs(distance_to_pixel - model_distance) / model_distance, 0.0, 1.0);
    x = max(1.0 - x, 0.0);
    x = 1.0 - pow(x, 1.0/10.0);

    vec2 radius = vec2(size/img_size.x, size/img_size.y);
    
    vec4 blurred_colour = texture(colour_tex, ftexcoord);
    
    for( float d=0.0; d<pi_times_2; d+= pi_times_2/directions)
		for(float i=1.0/quality; i<=1.0; i+=1.0/quality)
			blurred_colour += texture( colour_tex, ftexcoord + vec2(cos(d),sin(d))*radius*x*i);	
    
    // Output to screen
    blurred_colour /= quality * directions;// - 15.0;

    frag_colour.rgb = blurred_colour.rgb;//vec3(mix(unblurred_colour, blurred_colour, x));
    frag_colour.a = 1.0;
}


