#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;


uniform int clip_plane_enabled;
uniform vec4 clip_plane;
//out float gl_ClipDistance[1];


out VS_OUT {
    
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;

    vec3 untransformed_normal;
    vec3 untransformed_position;

    vec4 ProjTexCoord;

} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

mat4 mvp = projection * view * model;

uniform mat4 ProjectorMatrix;



void main()
{
    vs_out.untransformed_normal = aNormal;
    vs_out.untransformed_position = aPos;

    vec4 pos4 = vec4(aPos, 1.0);
    vs_out.ProjTexCoord = ProjectorMatrix * (model * pos4);

    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.TexCoords = aTexCoords;
    
    gl_Position = mvp * vec4(aPos, 1.0);

//    if(clip_plane_enabled == 1)
//        gl_ClipDistance[0] = dot(model * vec4(aPos, 1.0), clip_plane);
//    else
//        gl_ClipDistance[0] = 1.0f;


}

