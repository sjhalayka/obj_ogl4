#version 430 core
out vec4 FragColor;

in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 untransformed_normal;
    vec3 untransformed_position;

    vec4 ProjTexCoord;
} fs_in;




const int max_num_lights = 4;
layout (binding=0) uniform sampler2D projectorTexture;
layout (binding=1) uniform sampler2D diffuseTexture;
layout (binding=2) uniform sampler2D specularTexture;


uniform samplerCube depthMaps[max_num_lights];

uniform vec3 lightPositions[max_num_lights];
uniform vec3 lightColours[max_num_lights];
uniform int lightEnabled[max_num_lights];
uniform int lightShadowCaster[max_num_lights];

uniform int do_proj_tex = 0;



uniform float far_plane;
uniform int shadows = 1;
uniform int flat_draw = 0;
uniform vec3 flat_colour;


uniform int specular_only = 0;


vec3 MaterialKd = vec3(1.0, 1.0, 1.0);
vec3 MaterialKs = vec3(1.0, 0.5, 0.0);
vec3 MaterialKa = vec3(0.0, 0.025, 0.075);
float MaterialShininess = 1;


vec3 phongModelDiffAndSpec(bool do_specular, vec3 lp, int index)
{
    vec3 n = fs_in.Normal;
    vec3 s = normalize(vec3(lightPositions[index].xyz) - fs_in.untransformed_position);
    vec3 v = normalize(-fs_in.untransformed_position.xyz);
    vec3 r = reflect( -s, n );
    float sDotN = max( dot(s,n), 0.0 );
    vec3 diffuse = lightColours[index] * texture(diffuseTexture, fs_in.TexCoords).rgb * sDotN;
    vec3 spec = vec3(0.0);

    if( sDotN > 0.0 )
    {
        spec.x = MaterialKs.x * pow( max( dot(r,v), 0.0 ), MaterialShininess );
        spec.y = MaterialKs.y * pow( max( dot(r,v), 0.0 ), MaterialShininess );
        spec.z = MaterialKs.z * pow( max( dot(r,v), 0.0 ), MaterialShininess );
    }

    vec3 n2 = fs_in.Normal;
    vec3 s2 = normalize(vec3(-lightPositions[index]) - fs_in.untransformed_position);
    vec3 v2 = normalize(-fs_in.untransformed_position.xyz);
    vec3 r2 = reflect( -s2, n2 );
    float sDotN2 = max( dot(s2,n2)*0.5f, 0.0 );
    vec3 diffuse2 = lightColours[index]*0.25 * MaterialKa * sDotN2;

    float k = (1.0 - sDotN)/2.0;
    vec3 ret = diffuse + diffuse2 + MaterialKa*k;

    if(do_specular)
        ret = ret + spec;
    
    return ret;
}











// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

/*
float get_shadow(vec3 lp, samplerCube dmap)
{
    // get vector between fragment position and light position
    vec3 fragToLight = fs_in.FragPos - lp;
    // use the fragment to light vector to sample from the depth map    
    // float closestDepth = texture(dmap, fragToLight).r;
    // it is currently in linear range between [0,1], let's re-transform it back to original depth value
    // closestDepth *= far_plane;
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    // float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
    // float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;
    // PCF
    // float shadow = 0.0;
    // float bias = 0.05; 
    // float samples = 4.0;
    // float offset = 0.1;
    // for(float x = -offset; x < offset; x += offset / (samples * 0.5))
    // {
        // for(float y = -offset; y < offset; y += offset / (samples * 0.5))
        // {
            // for(float z = -offset; z < offset; z += offset / (samples * 0.5))
            // {
                // float closestDepth = texture(depthMap, fragToLight + vec3(x, y, z)).r; // use lightdir to lookup cubemap
                // closestDepth *= far_plane;   // Undo mapping [0;1]
                // if(currentDepth - bias > closestDepth)
                    // shadow += 1.0;
            // }
        // }
    // }
    // shadow /= (samples * samples * samples);
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(viewPos - fs_in.FragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(dmap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    //FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}
*/









float get_shadow(vec3 lp, samplerCube dmap)
{
	// get vector between fragment position and light position
	vec3 fragToLight = fs_in.FragPos - lp;
	// ise the fragment to light vector to sample from the depth map    
	float closestDepth = texture(dmap, fragToLight).r;
	// it is currently in linear range between [0,1], let's re-transform it back to original depth value
	closestDepth *= far_plane;
	// now get current linear depth as the length between the fragment and light position
	float currentDepth = length(fragToLight);
	// test for shadows
	float bias = 0.5; // we use a much larger bias since depth is now in [near_plane, far_plane] range
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	shadow = 1 - shadow;

	return shadow;

}





void main()
{
    if(flat_draw == 1)
	{
		FragColor = vec4(flat_colour, 1.0);
		return;
	}

	if(specular_only == 1)
	{
		FragColor = texture(specularTexture, fs_in.TexCoords);
		
		vec3 n =  fs_in.untransformed_normal;
		vec3 n2 = vec3(0, 1, 0);

		if(dot(n, n2) < 0.95)
			FragColor = vec4(0, 0, 0, 1);
	
		return;
	}



	MaterialKs *= texture(specularTexture, fs_in.TexCoords).rgb;


	int num_shadows = 0;

	for (int i = 0; i < max_num_lights; i++)
	{
		if (lightEnabled[i] == 0)
			continue;

		float s = get_shadow(lightPositions[i], depthMaps[i]);
		
		if(lightShadowCaster[i] == 0)
			s = 1.0;

		if(s == 0)
			num_shadows++;
	}

	vec3 diffAndSpec = vec3(0, 0, 0);





	if (num_shadows == 0)
	{
		for (int i = 0; i < max_num_lights; i++)
		{
			if (lightEnabled[i] == 0)
				continue;

			diffAndSpec += phongModelDiffAndSpec(true, lightPositions[i], i);
		}
	}
	else
	{
		for (int i = 0; i < max_num_lights; i++)
		{
			if (lightEnabled[i] == 0)
				continue;

			float s = 0;

			if(lightShadowCaster[i] == 0)
				s = 1;
			else
				s = get_shadow(lightPositions[i], depthMaps[i]);

			vec3 phong_contrib = phongModelDiffAndSpec(true, lightPositions[i], i);
			vec3 shadow_contrib = s * phongModelDiffAndSpec(false, lightPositions[i], i);
			diffAndSpec += mix(phong_contrib, shadow_contrib, 0.50);
		}
	}
	
	FragColor = vec4(diffAndSpec, 1.0);
	

    if(do_proj_tex == 1 && fs_in.ProjTexCoord.z > 0.0 )
    {
        vec4 projTexColor = textureProj(projectorTexture, fs_in.ProjTexCoord);

        if(projTexColor.a != 0)
            FragColor = projTexColor;
    }

    FragColor = pow(FragColor, vec4(1.0 / 2.2));

}