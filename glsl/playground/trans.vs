#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 UV;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalM;


void main()
{
    gl_Position = projection * view * model * vec4(aPosition.xyz, 1.0);
    vs_out.FragPos = (model * vec4(aPosition.xyz, 1.0)).xyz;
    vs_out.Normal = normalize(normalM * aNormal);
    vs_out.UV = aUV;
}