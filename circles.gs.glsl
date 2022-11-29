#version 430
layout (points) in;                              
layout (triangle_strip, max_vertices = 34) out; // 16 steps


uniform int img_width;
uniform int img_height;
uniform int cam_factor;
uniform vec3 camera_pos; // assumes that the lookat is effectively vec3(0, 0, 0)

uniform float line_thickness;

const vec2  u_viewportSize = vec2(img_width, img_height);
const float u_thickness = cam_factor * line_thickness;
const float steps = 16;
const float pi = 4.0*atan(1.0);

void main()
{
    const vec4 position = gl_in[0].gl_Position;
    const vec2 offset = vec2(u_thickness / u_viewportSize);

    for( float phi = -pi/2; phi <= pi/2; phi += pi/steps)
    {
        const float x1 = offset.x * sin(phi);
        const float y1 = offset.y * cos(phi);

        gl_Position = position + vec4(x1 * position.w,  y1 * position.w, 0.0, 0.0);
        EmitVertex();

        gl_Position = position + vec4(x1 * position.w, -y1 * position.w, 0.0, 0.0);
        EmitVertex();
    }

    EndPrimitive();



    /*
    // draw a square, for debug purposes
    gl_Position = position + vec4(-offset.x * position.w, -offset.y * position.w, 0.0, 0.0);
    EmitVertex();   
    gl_Position = position + vec4( offset.x * position.w, -offset.y * position.w, 0.0, 0.0);
    EmitVertex();
    gl_Position = position + vec4(-offset.x * position.w,  offset.y * position.w, 0.0, 0.0);
    EmitVertex();
    gl_Position = position + vec4( offset.x * position.w,  offset.y * position.w, 0.0, 0.0);
    EmitVertex();

    EndPrimitive();
    */
}