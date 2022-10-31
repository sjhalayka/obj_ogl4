#version 430

uniform sampler2D regular_tex;
uniform sampler2D upside_down_tex;
uniform sampler2D reflectance_tex;
uniform sampler2D upside_down_white_mask_tex;

in vec2 ftexcoord;

layout(location = 0) out vec4 frag_colour;

void main()
{
   frag_colour = mix(texture(regular_tex, ftexcoord), texture(upside_down_tex, ftexcoord), texture(reflectance_tex, ftexcoord)*texture(upside_down_white_mask_tex, ftexcoord));                 
   frag_colour.a = 1.0;
}


