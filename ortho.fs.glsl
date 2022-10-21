#version 430

uniform sampler2D depth_tex; // texture uniform
uniform sampler2D colour_tex; // texture uniform


in vec2 ftexcoord;

layout(location = 0) out vec4 frag_colour;

void main()
{
	frag_colour = texture(colour_tex, ftexcoord);
	frag_colour.rgb = 1.0 - frag_colour.rgb;

//	vec4 depth_colour = texture(depth_tex, ftexcoord);
//	depth_colour.r = pow(depth_colour.r, 100.0);
//	depth_colour.g = pow(depth_colour.g, 100.0);
//	depth_colour.b = pow(depth_colour.b, 100.0);
//	frag_colour.rgb = 1.0 - depth_colour.rgb;



};