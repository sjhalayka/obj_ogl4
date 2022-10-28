

#version 430 core
out vec4 FragColor;

in VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
	vec3 mvPosition;
} fs_in;




const int max_num_lights = 3;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;

uniform samplerCube depthMaps[max_num_lights];

uniform vec3 lightPositions[max_num_lights];
uniform vec3 lightColours[max_num_lights];
uniform int lightEnabled[max_num_lights];








uniform vec3 viewPos;

uniform float far_plane;
uniform int shadows = 1;
uniform int flat_draw = 0;
uniform vec3 flat_colour;


vec3 MaterialKd = vec3(1.0, 1.0, 1.0);
vec3 MaterialKs = vec3(1.0, 0.5, 0.0);
vec3 MaterialKa = vec3(0.0, 0.025, 0.075);
float MaterialShininess = 0.01;


vec3 phongModelDiffAndSpec(bool do_specular, vec3 lp, int index)
{
	vec3 n = fs_in.Normal;
	vec3 s_light1 = normalize(vec3(lp.xyz) - fs_in.mvPosition);

	vec3 v = normalize(-fs_in.mvPosition.xyz);
	vec3 r_light1 = reflect(-s_light1, n);

	float sDotN_light1 = max(dot(s_light1, n), 0.0);

	vec3 diffuse_light1 = lightColours[index] * texture(diffuseTexture, fs_in.TexCoords).rgb * sDotN_light1;

	vec3 spec_light1 = vec3(0.0);

	if (sDotN_light1 > 0.0)
	{
		spec_light1.x = MaterialKs.x * pow(max(dot(r_light1, v), 0.0), MaterialShininess);
		spec_light1.y = MaterialKs.y * pow(max(dot(r_light1, v), 0.0), MaterialShininess);
		spec_light1.z = MaterialKs.z * pow(max(dot(r_light1, v), 0.0), MaterialShininess);
	}


	vec3 n2 = fs_in.Normal;

	vec3 s2_light1 = normalize(vec3(-lp) - fs_in.mvPosition);

	vec3 v2 = normalize(-fs_in.mvPosition.xyz);

	vec3 r2_light1 = reflect(-s2_light1, n2);



	float sDotN2_light1 = max(dot(s2_light1, n2) * 0.5f, 0.0);




	vec3 diffuse2_light1 = lightColours[index] * 0.25 * MaterialKa * sDotN2_light1;



	float k_light1 = (1.0 - sDotN_light1) / 2.0;



	vec3 ret_light1 = diffuse_light1 + diffuse2_light1 + MaterialKa * k_light1;

	if (do_specular)
	{
		ret_light1 = ret_light1 + spec_light1;
	}

	return (ret_light1);// + ret_light2);///2.0;
}





float get_shadow(vec3 lp, samplerCube dmap)
{
	float shadow = 0.0;

	// get vector between fragment position and light position
	vec3 fragToLight = fs_in.FragPos - lp;
	// ise the fragment to light vector to sample from the depth map    
	float closestDepth = texture(dmap, fragToLight).r;
	// it is currently in linear range between [0,1], let's re-transform it back to original depth value
	closestDepth *= far_plane;
	// now get current linear depth as the length between the fragment and light position
	float currentDepth = length(fragToLight);
	// test for shadows
	float bias = 0.05; // we use a much larger bias since depth is now in [near_plane, far_plane] range
	shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	shadow = 1 - shadow;

	return shadow;

}



void main()
{
	MaterialKs *= texture(specularTexture, fs_in.TexCoords).rgb;

	if(flat_draw == 1)
	{
		FragColor = vec4(flat_colour, 1.0);
		return;
	}

	float brightest_contribution = 0.0;

	for (int i = 0; i < max_num_lights; i++)
	{
		if (lightEnabled[i] == 0)
			continue;

		float s = get_shadow(lightPositions[i], depthMaps[i]);

		if (s > brightest_contribution)
			brightest_contribution = s;
	}

	vec3 diffAndSpec = vec3(0, 0, 0);

	if (brightest_contribution == 1.0)
	{
		for (int i = 0; i < max_num_lights; i++)
		{
			if (lightEnabled[i] == 0)
				continue;

			diffAndSpec += phongModelDiffAndSpec(true, lightPositions[i], i++);
		}

		FragColor = vec4(diffAndSpec, 1.0);// + vec4(diffAndSpec * shadow + MaterialKa*(1.0 - shadow), 1.0);
	}
	else
	{
		for (int i = 0; i < max_num_lights; i++)
		{
			if (lightEnabled[i] == 0)
				continue;

			diffAndSpec += phongModelDiffAndSpec(false, lightPositions[i], i++);
		}

		FragColor = vec4(diffAndSpec * brightest_contribution + MaterialKa * (1.0 - brightest_contribution), 1.0) + vec4(diffAndSpec, 1.0) + vec4(diffAndSpec * brightest_contribution + MaterialKa * (1.0 - brightest_contribution), 1.0);
		FragColor /= 3;
	}

	FragColor = pow(FragColor, vec4(1.0 / 2.2));


	return;
}

