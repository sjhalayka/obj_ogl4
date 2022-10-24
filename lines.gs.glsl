#version 430
layout (triangles) in;                              // now we can access 2 vertices
layout (triangle_strip, max_vertices = 4) out;  // always (for now) producing 2 triangles (so 4 vertices)

uniform int img_width;
uniform int img_height; 


vec2 u_viewportSize = vec2(img_width, img_height);


float u_thickness = 10;

in VS_OUT {
vec4 mColor;
vec2 gtexcoord;
} gs_in[];




out vec2 ftexcoord;



void main(void)
{
    for (int i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position;

        ftexcoord = gs_in[i].gtexcoord;

        EmitVertex();
    }

    EndPrimitive();
}  

