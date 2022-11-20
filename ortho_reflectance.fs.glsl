#version 430

uniform sampler2D regular_tex;
uniform sampler2D upside_down_tex;
uniform sampler2D reflectance_tex;
uniform sampler2D upside_down_white_mask_tex;
uniform sampler2D glowmap_tex;
uniform sampler2D depth_tex;

in vec2 ftexcoord;

uniform int img_width;
uniform int img_height;
uniform int cam_factor; 

vec2 img_size = vec2(img_width, img_height);

layout(location = 0) out vec4 frag_colour;

void main()
{


    // for debug purposes
//  frag_colour = texture( regular_tex, ftexcoord);

//  return;


   const float pi_times_2 = 6.28318530718; // Pi*2
    
    float directions = 16.0; // BLUR directions (Default 16.0 - More is better but slower)
    float quality = 4.0; // BLUR quality (Default 4.0 - More is better but slower)
    float size = 20.0; // BLUR size (radius)
    vec2 radius = vec2(size/img_size.x * cam_factor, size/img_size.y * cam_factor);




   int count = 0;

   vec4 glowmap_blurred_colour = texture(glowmap_tex, ftexcoord);
   count++;
   
    for( float d=0.0; d<pi_times_2; d+= pi_times_2/directions)
    {
		for(float i=1.0/quality; i<=1.0; i+=1.0/quality)
        {
            vec4 temp_colour = texture( glowmap_tex, ftexcoord + vec2(cos(d),sin(d))*radius*i);

            if(temp_colour.xyz != vec3(0, 0, 0))
            {
        	    glowmap_blurred_colour += temp_colour;
                count++;
            }
        }
    }
    
    // Output to screen
    glowmap_blurred_colour /= count;

    



 








   count = 0;

   vec4 blurred_colour = texture(upside_down_tex, ftexcoord);
   count++;
   
    for( float d=0.0; d<pi_times_2; d+= pi_times_2/directions)
    {
		for(float i=1.0/quality; i<=1.0; i+=1.0/quality)
        {
            vec4 temp_colour = texture( upside_down_tex, ftexcoord + vec2(cos(d),sin(d))*radius*i);

            if(temp_colour.xyz != vec3(0, 0, 0))
            {
        	    blurred_colour += temp_colour;
                count++;
            }
        }
    }
    
    // Output to screen
    blurred_colour /= count;
    

    vec4 upside_down_colour = blurred_colour;

    // Make glossiness out of the reflectance texture (the more reflective, the sharper the image)
    upside_down_colour = mix(upside_down_colour, texture(upside_down_tex, ftexcoord), texture(reflectance_tex, ftexcoord));




    count = 0;
    blurred_colour = texture(upside_down_white_mask_tex, ftexcoord);
    count++;

    for( float d=0.0; d<pi_times_2; d+= pi_times_2/directions)
    {
		for(float i=1.0/quality; i<=1.0; i+=1.0/quality)
	    {
            blurred_colour += texture( upside_down_white_mask_tex, ftexcoord + vec2(cos(d),sin(d))*radius*i);
            count++;
        }
    }

    blurred_colour /= count;

    vec4 upside_down_white_mask = blurred_colour;


    // Make glossiness out of the reflectance texture (the more reflective, the sharper the image)
    upside_down_white_mask = mix(upside_down_white_mask, texture(upside_down_white_mask_tex, ftexcoord), texture(reflectance_tex, ftexcoord));

    vec4 final_colour = mix(texture(regular_tex, ftexcoord), upside_down_colour, texture(reflectance_tex, ftexcoord)*upside_down_white_mask);

    // Bloom mixer
    frag_colour = mix(final_colour, glowmap_blurred_colour, glowmap_blurred_colour);                 
}


