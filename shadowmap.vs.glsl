#version 430

layout (location=0) in vec3 position;
layout (location=1) in vec3 normal;
layout (location=2) in vec2 texcoord;

out vec2 ftexcoord;
out vec3 Position;
out vec3 Normal;
out vec4 ShadowCoord;
out vec4 ShadowCoord2;

out vec3 Untransformed_Position;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

uniform mat3 NormalMatrix;
uniform mat4 ShadowMatrix;
uniform mat4 ShadowMatrix2;

void main()
{
    mat4 mv = ViewMatrix * ModelMatrix;
    mat4 mvp = ProjectionMatrix*mv;

    Position = (mv * vec4(position,1.0)).xyz;
    Normal = normalize( NormalMatrix * normal );
    ShadowCoord = ShadowMatrix * vec4(position, 1.0);
    ShadowCoord2 = ShadowMatrix2 * vec4(position, 1.0);

    Untransformed_Position = position;

    gl_Position = mvp * vec4(position, 1.0);
    ftexcoord = texcoord;
}
