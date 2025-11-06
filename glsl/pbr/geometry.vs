#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 vsout_frag_pos;
out vec3 vsout_normal;
out vec2 vsout_uv;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat3 normalMatrix;

void main()
{
    vsout_uv = aTexCoords;
    vsout_frag_pos = vec3(model * vec4(aPos, 1.0));
    vsout_normal = normalMatrix * aNormal;   

    gl_Position =  projection * view * vec4(vsout_frag_pos, 1.0);
}