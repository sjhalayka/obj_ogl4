#version 430
layout (points) in;                              // now we can access 2 vertices
layout (triangle_strip, max_vertices = 4) out;  // always (for now) producing 2 triangles (so 4 vertices)


uniform int img_width;
uniform int img_height;
uniform int cam_factor;
uniform vec3 camera_pos; // assumes that the lookat is effectively vec3(0, 0, 0)

uniform float line_thickness;

vec2  u_viewportSize = vec2(img_width, img_height);
float u_thickness = cam_factor * line_thickness;

    


void main()
{
    vec4 position = gl_in[0].gl_Position;

    vec2 offset = vec2(u_thickness / u_viewportSize);

    gl_Position = position + vec4(-offset.x* position.w, -offset.y* position.w, 0.0, 0.0);
    EmitVertex();   
    gl_Position = position + vec4( offset.x* position.w, -offset.y* position.w, 0.0, 0.0);
    EmitVertex();
    gl_Position = position + vec4(-offset.x* position.w,  offset.y* position.w, 0.0, 0.0);
    EmitVertex();
    gl_Position = position + vec4(offset.x* position.w, offset.y* position.w, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
}