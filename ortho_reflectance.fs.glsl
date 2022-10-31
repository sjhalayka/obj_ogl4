#version 430

uniform sampler2D regular_tex; // texture uniform
uniform sampler2D upside_down_tex; // texture uniform
uniform sampler2D reflectance; // texture uniform
uniform sampler2D upside_down_white_mask;

in vec2 ftexcoord;

layout(location = 0) out vec4 frag_colour;

void main()
{
    frag_colour = mix(texture(regular_tex, ftexcoord), 
                    texture(upside_down_tex, ftexcoord),
                    texture(reflectance, ftexcoord)*texture(upside_down_white_mask, ftexcoord));

    frag_colour.a = 1.0;
}


