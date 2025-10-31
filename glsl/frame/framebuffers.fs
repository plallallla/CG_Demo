#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{    
    FragColor = texture(texture1, TexCoords);
    BrightColor = vec4(1.0-FragColor.r,1.0-FragColor.g,1.0-FragColor.b,10);
}