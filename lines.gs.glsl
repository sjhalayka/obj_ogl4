#version 430

layout (lines) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat4    u_model_matrix;
uniform mat4    u_view_matrix;
uniform mat4    u_projection_matrix;
float   u_thickness = 4;

void main()
{
    float r = u_thickness / 2;

    mat4 mv = u_view_matrix * u_model_matrix;
    vec4 p1 = mv * gl_in[0].gl_Position;
    vec4 p2 = mv * gl_in[1].gl_Position;

    vec2 dir = normalize(p2.xy - p1.xy);
    vec2 normal = vec2(dir.y, -dir.x);

    vec4 offset1, offset2;
    offset1 = vec4(normal * r, 0, 0);
    offset2 = vec4(normal * r, 0, 0);

    vec4 coords[4];
    coords[0] = p1 + offset1;
    coords[1] = p1 - offset1;
    coords[2] = p2 + offset2;
    coords[3] = p2 - offset2;

    for (int i = 0; i < 4; ++i) {
        coords[i] = u_projection_matrix * coords[i];
        gl_Position = coords[i];
        EmitVertex();
    }
    EndPrimitive();
}