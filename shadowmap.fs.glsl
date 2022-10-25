#version 430

uniform sampler2DShadow shadow_map;
uniform sampler2DShadow shadow_map2;

uniform sampler2D colour_tex;

in vec2 ftexcoord;
in vec3 Position;
in vec3 Normal;
in vec4 ShadowCoord;
in vec4 ShadowCoord2;


uniform vec4 LightPosition; // in view space
uniform vec4 LightPosition_Untransformed; // in world space

uniform vec4 LightPosition2; // in view space
uniform vec4 LightPosition_Untransformed2; // in world space


uniform int flat_colour = 0;

vec3 LightIntensity = vec3(1.0, 1.0, 1.0);

uniform vec3 MaterialKd;

vec3 MaterialKs = vec3(1.0, 0.5, 0.0);
vec3 MaterialKa = vec3(0.0, 0.025, 0.075);
float MaterialShininess = 10.0;

layout (location = 0) out vec4 frag_colour;


vec4 t = texture(colour_tex, ftexcoord);


vec3 phongModelDiffAndSpec(bool do_specular)
{
    vec3 n = Normal;
    vec3 s_light1 = normalize(vec3(LightPosition.xyz) - Position);
    vec3 s_light2 = normalize(vec3(LightPosition2.xyz) - Position);

    vec3 v = normalize(-Position.xyz);
    vec3 r_light1 = reflect( -s_light1, n );
    vec3 r_light2 = reflect( -s_light2, n );

    float sDotN_light1 = max( dot(s_light1,n), 0.0 );
    float sDotN_light2 = max( dot(s_light2,n), 0.0 );


    vec3 diffuse_light1 = LightIntensity * texture(colour_tex, ftexcoord).rgb * sDotN_light1;
    vec3 diffuse_light2 = LightIntensity * texture(colour_tex, ftexcoord).rgb * sDotN_light2;

    vec3 spec_light1 = vec3(0.0);
    vec3 spec_light2 = vec3(0.0);

    if( sDotN_light1 > 0.0 )
    {
        spec_light1.x = MaterialKs.x * pow( max( dot(r_light1,v), 0.0 ), MaterialShininess );
        spec_light1.y = MaterialKs.y * pow( max( dot(r_light1,v), 0.0 ), MaterialShininess );
        spec_light1.z = MaterialKs.z * pow( max( dot(r_light1,v), 0.0 ), MaterialShininess );
    }

   if( sDotN_light2 > 0.0 )
    {
        spec_light2.x = MaterialKs.x * pow( max( dot(r_light2,v), 0.0 ), MaterialShininess );
        spec_light2.y = MaterialKs.y * pow( max( dot(r_light2,v), 0.0 ), MaterialShininess );
        spec_light2.z = MaterialKs.z * pow( max( dot(r_light2,v), 0.0 ), MaterialShininess );
    }



    vec3 n2 = Normal;


    vec3 s2_light1 = normalize(vec3(-LightPosition) - Position);
    vec3 s2_light2 = normalize(vec3(-LightPosition2) - Position);

    vec3 v2 = normalize(-Position.xyz);
    
    vec3 r2_light1 = reflect( -s2_light1, n2 );
    vec3 r2_light2 = reflect( -s2_light2, n2 );
    
    
    
    float sDotN2_light1 = max( dot(s2_light1,n2)*0.5f, 0.0 );
    float sDotN2_light2 = max( dot(s2_light2,n2)*0.5f, 0.0 );




    vec3 diffuse2_light1 = LightIntensity*0.25 * MaterialKa * sDotN2_light1;
    vec3 diffuse2_light2 = LightIntensity*0.25 * MaterialKa * sDotN2_light2;



    float k_light1 = (1.0 - sDotN_light1)/2.0;
    float k_light2 = (1.0 - sDotN_light2)/2.0;



    vec3 ret_light1 = diffuse_light1 + diffuse2_light1 + MaterialKa*k_light1;
    vec3 ret_light2 = diffuse_light2 + diffuse2_light2 + MaterialKa*k_light2;

    if(do_specular)
    {
        ret_light1 = ret_light1 + spec_light1;
        ret_light2 = ret_light2 + spec_light1;
    }

    return (ret_light1);// + ret_light2);///2.0;
}

subroutine void RenderPassType();
subroutine uniform RenderPassType RenderPass;

subroutine (RenderPassType)
void shadeWithShadow()
{


    if(flat_colour == 1)
    {
        frag_colour = vec4(MaterialKd, 1.0);
        return;
    }


    float shadow1 = 1.0;
    float shadow2 = 1.0;
    
   if( ShadowCoord.z >= 0.0 )
    {
      shadow1 = textureProj(shadow_map, ShadowCoord);
    }
    
   if( ShadowCoord2.z >= 0.0 )
    {
      shadow2 = textureProj(shadow_map2, ShadowCoord2);
    }

    float shadow = 1.0;

    if(shadow1 != 1.0 || shadow2 != 1.0)
        shadow = 0.25;

    vec3 diffAndSpec;
    
    if(shadow == 1.0)
    {
        diffAndSpec = phongModelDiffAndSpec(true);
        frag_colour = vec4(diffAndSpec, 1.0);// + vec4(diffAndSpec * shadow + MaterialKa*(1.0 - shadow), 1.0);
    }
    else
    {
        diffAndSpec = phongModelDiffAndSpec(false);
        frag_colour = vec4(diffAndSpec * shadow + MaterialKa*(1.0 - shadow), 1.0) + vec4(diffAndSpec, 1.0) + vec4(diffAndSpec * shadow + MaterialKa*(1.0 - shadow), 1.0);
        frag_colour /= 3;
    }

    frag_colour = pow( frag_colour, vec4(1.0 / 2.2) );



    /*
    // posterize https://www.geeks3d.com/20091027/shader-library-posterization-post-processing-effect-glsl/
        float gamma = 1.0; // 0.6
    float numColors = 4.0; // 8.0

    vec3 c = frag_colour.rgb;
    c = pow(c, vec3(gamma, gamma, gamma));
    c = c * numColors;
    c = floor(c);
    c = c / numColors;
    c = pow(c, vec3(1.0/gamma));
    frag_colour = vec4(c, 1.0);
    */
}

subroutine (RenderPassType)
void recordDepth()
{
    // Do nothing, depth will be written automatically
}

void main() {
    // This will call either shadeWithShadow or recordDepth
    RenderPass();
}