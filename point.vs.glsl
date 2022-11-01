#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 mvPosition;
    vec3 untransformed_normal;
    vec3 untransformed_position;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform bool reverse_normals = false;

void main()
{
    vs_out.untransformed_normal = aNormal;
    vs_out.untransformed_position = aPos;


    mat4 mv = view * model;
    mat4 mvp = projection * view * model;


    vs_out.mvPosition = (mv * vec4(aPos,1.0)).xyz;

    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    if(reverse_normals) // a slight hack to make sure the outer large cube displays lighting from the 'inside' instead of the default 'outside'.
        vs_out.Normal = transpose(inverse(mat3(model))) * (-1.0 * aNormal);
    else
        vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}

