#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    // FragColor = vec4(col.r, col.g, col.b, 1.0);
    FragColor = vec4(1-col.r, 1-col.g, 1-col.b, 1.0);
} 