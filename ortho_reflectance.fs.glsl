#version 430

uniform sampler2D regular_tex;
uniform sampler2D upside_down_tex;
uniform sampler2D reflectance_tex;
uniform sampler2D upside_down_white_mask_tex;
uniform sampler2D depth_tex;

in vec2 ftexcoord;

uniform int img_width;
uniform int img_height;
uniform int cam_factor; 

vec2 img_size = vec2(img_width, img_height);

layout(location = 0) out vec4 frag_colour;

void main()
{
   const float pi_times_2 = 6.28318530718; // Pi*2
    
    float directions = 16.0; // BLUR directions (Default 16.0 - More is better but slower)
    float quality = 10.0; // BLUR quality (Default 4.0 - More is better but slower)
    float size = 16.0; // BLUR size (radius)
        vec2 radius = vec2(size/img_size.x * cam_factor, size/img_size.y * cam_factor);



   vec4 blurred_colour = texture(upside_down_tex, ftexcoord);
    
    for( float d=0.0; d<pi_times_2; d+= pi_times_2/directions)
		for(float i=1.0/quality; i<=1.0; i+=1.0/quality)
			blurred_colour += texture( upside_down_tex, ftexcoord + vec2(cos(d),sin(d))*radius*i);	
    
    // Output to screen
    blurred_colour /= quality * directions - 15.0;

	vec4 upside_down_colour = blurred_colour;


frag_colour =  mix(texture(regular_tex, ftexcoord), upside_down_colour, texture(reflectance_tex, ftexcoord)*texture(upside_down_white_mask_tex, ftexcoord));                 
return;


}


