#version 430

uniform sampler2D depth_tex; // texture uniform
uniform sampler2D colour_tex; // texture uniform

uniform int img_width;
uniform int img_height;

vec2 img_size = vec2(img_width, img_height);

in vec2 ftexcoord;

layout(location = 0) out vec4 frag_colour;


float gauss(float x, float x0, float sx){
    
    float arg = x-x0;
    arg = -1./2.*arg*arg/sx;
    
    float a = 1./(pow(2.*3.1415*sx, 0.5));
    
    return a*exp(arg);
}


// https://www.shadertoy.com/view/Xltfzj
void main()
{
    const float pi_times_2 = 6.28318530718; // Pi*2
    
    float directions = 4.0; // BLUR directions (Default 16.0 - More is better but slower)
    float quality = 10.0; // BLUR quality (Default 4.0 - More is better but slower)
    float size = 8.0; // BLUR size (radius)
   
    vec2 radius = vec2(size/img_size.x, size/img_size.y);
    
    vec4 blurred_colour = texture(colour_tex, ftexcoord);
    
    for( float d=0.0; d<pi_times_2; d+= pi_times_2/directions)
		for(float i=1.0/quality; i<=1.0; i+=1.0/quality)
			blurred_colour += texture( colour_tex, ftexcoord + vec2(cos(d),sin(d))*radius*i);		
    
    // Output to screen
    blurred_colour /= quality * directions;// - 15.0;

	vec4 unblurred_colour = texture(colour_tex, ftexcoord);

	float depth_colour = texture(depth_tex, ftexcoord).r;
	depth_colour = pow(depth_colour, 100.0);

    //depth_colour = pow(gauss(0.0, 1.0, depth_colour), 1.0);//smoothstep(0.0, 1.0, depth_colour);//pow(abs( depth_colour - 0.5)*2.0, 1100.0);

    frag_colour.rgb = vec3(mix(unblurred_colour, blurred_colour, depth_colour));
    frag_colour.a = 1.0;
}


