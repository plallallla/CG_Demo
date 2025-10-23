#version 330 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;
layout (location = 2) in vec2 aOffset;

// uniform vec2 offsets[100];

out vec3 fColor;

void main()
{
    // gl_Position = vec4(aPos + offsets[gl_InstanceID], 0.0, 1.0);
    gl_Position = vec4(aPos + aOffset, 0.0, 1.0);
    fColor = aColor;
}