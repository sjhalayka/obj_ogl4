#version 430
layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 texcoord;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

out VS_OUT {
    vec4 mColor;
    vec2 gtexcoord;
} vs_out;

void main(void)
{
    vs_out.mColor = vec4(1, 0, 0, 1);
    vs_out.gtexcoord = texcoord;

    gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(position, 1.0);
}