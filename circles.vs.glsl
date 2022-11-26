#version 430
layout (location = 0) in vec3 position;

uniform mat4  u_modelviewprojection_matrix;

void main()
{
    gl_Position = u_modelviewprojection_matrix * vec4(position, 1.0);
}