#version 430

uniform sampler2D depth_tex; // texture uniform
uniform sampler2D colour_tex; // texture uniform

uniform int img_width;
uniform int img_height;
uniform float model_distance;
uniform float near;
uniform float far;
uniform int screenshot_mode;
uniform int cam_factor;

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

vec4 get_blurred_pixel(vec2 tc)
{   
	float depth_colour = texture(depth_tex, tc).r;

    float distance_to_pixel = to_distance(depth_colour);
    
    float x = clamp((distance_to_pixel - model_distance) / model_distance, 0.0, 1.0);
   
    // tent function
    if(x > 0.5)
         x = 1.0 - x;

    x = 1.0 - pow(x, 1.0/10.0);

   const float pi_times_2 = 6.28318530718; // Pi*2
    
    float directions = 16.0; // BLUR directions (Default 16.0 - More is better but slower)
    float quality = 10.0; // BLUR quality (Default 4.0 - More is better but slower)
    float size = 8.0; // BLUR size (radius)


    vec2 radius = vec2(0, 0);
    
    if(screenshot_mode == 1)
        radius = vec2(size / img_width * cam_factor, size / img_height * cam_factor);
    else
        radius = vec2(size / img_width, size / img_height);


   vec4 blurred_colour = texture(colour_tex, tc);
    
    for(float d = 0.0; d < pi_times_2; d += pi_times_2 / directions)
		for(float i = 1.0 / quality; i <= 1.0; i += 1.0/quality)
			blurred_colour += texture(colour_tex, tc + vec2(cos(d), sin(d)) * radius * x * i);	
    
    // Output to screen
    blurred_colour /= quality * directions - 15.0;

    return blurred_colour;
}

// https://www.shadertoy.com/view/Xltfzj
// https://www.blitzcoder.org/forum/topic.php?id=124
void main()
{
    frag_colour.rgb = get_blurred_pixel(ftexcoord).rgb;
    frag_colour.a = 1.0;
}


