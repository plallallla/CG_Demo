//基础着色 应用纹理
#version 330 core

layout (location = 0) out vec4 frag_color;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 UV;
} vs_out;

uniform sampler2D diff;

void main()
{
    frag_color = texture(diff, vs_out.UV);
}