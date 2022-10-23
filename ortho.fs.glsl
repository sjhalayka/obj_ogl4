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
        vec2 radius = vec2(size/img_size.x, size/img_size.y);

   vec4 blurred_colour = texture(colour_tex, tc);
    
    for( float d=0.0; d<pi_times_2; d+= pi_times_2/directions)
		for(float i=1.0/quality; i<=1.0; i+=1.0/quality)
			blurred_colour += texture( colour_tex, tc + vec2(cos(d),sin(d))*radius*x*i);	
    
    // Output to screen
    blurred_colour /= quality * directions - 15.0;

    return blurred_colour;
}

// https://www.shadertoy.com/view/Xltfzj
// https://www.blitzcoder.org/forum/topic.php?id=124
void main()
{
   //frag_colour.rgb = texture(colour_tex, ftexcoord).rgb;
    frag_colour.rgb = get_blurred_pixel(ftexcoord).rgb;
    frag_colour.a = 1.0f;

    return;

     float FXAA_SPAN_MAX = 8.0;
    float FXAA_REDUCE_MUL = 1.0/8.0;
    float FXAA_REDUCE_MIN = 1.0/128.0;

  vec2 fragCoord = vec2(ftexcoord.x * img_width, ftexcoord.y * img_height);
  vec2 inverseVP = vec2(1.0 / img_width, 1.0 / img_height);
  vec2 v_rgbNW = (fragCoord + vec2(-1.0, -1.0)) * inverseVP;
  vec2 v_rgbNE = (fragCoord + vec2(1.0, -1.0)) * inverseVP;
  vec2 v_rgbSW = (fragCoord + vec2(-1.0, 1.0)) * inverseVP;
  vec2 v_rgbSE = (fragCoord + vec2(1.0, 1.0)) * inverseVP;
  vec2 v_rgbM = vec2(fragCoord * inverseVP);

  vec3 nw_colour = get_blurred_pixel(v_rgbNW).rgb;
  vec3 ne_colour = get_blurred_pixel(v_rgbNE).rgb;
  vec3 sw_colour = get_blurred_pixel(v_rgbSW).rgb;
  vec3 se_colour = get_blurred_pixel(v_rgbSE).rgb;
  vec3 m_colour =  get_blurred_pixel(v_rgbM).rgb;

    vec3 luma=vec3(0.299, 0.587, 0.114);
    float lumaNW = dot(nw_colour, luma);
    float lumaNE = dot(ne_colour, luma);
    float lumaSW = dot(sw_colour, luma);
    float lumaSE = dot(se_colour, luma);
    float lumaM  = dot(m_colour,  luma);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max(
        (lumaNW + lumaNE + lumaSW + lumaSE) * (0.25 * FXAA_REDUCE_MUL),
        FXAA_REDUCE_MIN);

    float rcpDirMin = 1.0/(min(abs(dir.x), abs(dir.y)) + dirReduce);

        dir = min(vec2( FXAA_SPAN_MAX,  FXAA_SPAN_MAX),
          max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
          dir * rcpDirMin)) / img_size;


    vec3 rgbA = (1.0/2.0) * (
        get_blurred_pixel(ftexcoord + dir * (1.0/3.0 - 0.5)).xyz +
        get_blurred_pixel(ftexcoord + dir * (2.0/3.0 - 0.5)).xyz);
    vec3 rgbB = rgbA * (1.0/2.0) + (1.0/4.0) * (
        get_blurred_pixel(ftexcoord + dir * (0.0/3.0 - 0.5)).xyz +
        get_blurred_pixel(ftexcoord + dir * (3.0/3.0 - 0.5)).xyz);
    float lumaB = dot(rgbB, luma);

    if((lumaB < lumaMin) || (lumaB > lumaMax)){
        frag_colour.rgb=rgbA;
    }else{
        frag_colour.rgb=rgbB;
    }

    frag_colour.a = 1.0;
}


