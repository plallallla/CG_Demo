#version 330 core

// layout (location = 0) out vec4 FragColor;
// layout (location = 1) out vec4 BrightColor;

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{    
    FragColor = texture(texture_diffuse1, TexCoords);
    // BrightColor = vec4(1.0) - FragColor;
}