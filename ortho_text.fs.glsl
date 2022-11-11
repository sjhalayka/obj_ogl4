#version 430

uniform sampler2D tex; // texture uniform

in vec2 ftexcoord;

layout(location = 0) out vec4 frag_colour;

void main() 
{
   frag_colour = texture(tex, ftexcoord);
};