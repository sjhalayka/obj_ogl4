#version 430

uniform sampler2D regular_tex;
uniform sampler2D upside_down_tex;
uniform sampler2D reflectance_tex;
uniform sampler2D upside_down_white_mask_tex;
uniform sampler2D depth_tex;

in vec2 ftexcoord;

layout(location = 0) out vec4 frag_colour;

float near = 0.1; 
float far  = 25.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

float to_distance(float depth_colour)
{
    float dist = (2.0*near*far) / (far + near - depth_colour*(far - near));	
    return dist;
}

float to_depth(float dist)
{
   // float depth = (far*(dist - 2.0*near) + near*dist)/(dist*(far - near));
    
//	float depth = (dist - near) / (far - near);
	
	float depth = (1/dist - 1/near) / (1/far - 1/near);

	return depth;
}


void main()
{


frag_colour =  mix(texture(regular_tex, ftexcoord), texture(upside_down_tex, ftexcoord), texture(reflectance_tex, ftexcoord)*texture(upside_down_white_mask_tex, ftexcoord));                 
return;


	float d = texture(depth_tex, ftexcoord).r;
	float depth = gl_FragCoord.z;//to_depth(gl_FragCoord.z);



		
	frag_colour = vec4(vec3(depth), 1.0);


	return;

	if(depth < d)
	{
	 frag_colour = vec4(depth, 0, 0, 1);//mix(texture(regular_tex, ftexcoord), texture(upside_down_tex, ftexcoord), texture(reflectance_tex, ftexcoord)*texture(upside_down_white_mask_tex, ftexcoord));                 
	}

else
	 frag_colour = vec4(0, 0, 1, 1);


	 return;

  // frag_colour = texture(depth_tex, ftexcoord);//mix(texture(regular_tex, ftexcoord), texture(upside_down_tex, ftexcoord), texture(reflectance_tex, ftexcoord)*texture(upside_down_white_mask_tex, ftexcoord));                 

 //

  // gl_FragDepth = gl_FragCoord.z;
		
//	frag_colour = mix(texture(regular_tex, ftexcoord), texture(upside_down_tex, ftexcoord), texture(reflectance_tex, ftexcoord)*texture(upside_down_white_mask_tex, ftexcoord));

//frag_colour = vec4(vec3(gl_FragCoord.z), 1.0);
//frag_colour

   frag_colour.a = 1.0;
}


