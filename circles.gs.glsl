#version 430
layout (points) in;                              
layout (triangle_strip, max_vertices = 48) out;  


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

    float r_x = offset.x;
    float r_y = offset.y;

    const float pi = 4.0*atan(1.0);
    const float pi_times_2 = pi*2;
    
    float directions = 16;

    for( float phi = -pi/2; phi < pi/2; phi += pi/directions)
    {
        float x1 = r_x * sin(phi);
        float y1 = r_y * cos(phi);

        gl_Position = position + vec4(x1 * position.w, y1 * position.w, 0.0, 0.0);
        EmitVertex();

        gl_Position = position + vec4(x1 * position.w, -y1 * position.w, 0.0, 0.0);
        EmitVertex();
    }

    float x_last = r_x * sin(pi/2);
    float y_last = r_y * cos(pi/2);
    gl_Position = position + vec4(x_last * position.w, y_last * position.w, 0.0, 0.0);
    EmitVertex();

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