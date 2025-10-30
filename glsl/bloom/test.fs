#version 330 core
out vec4 FragColor;

in vec3 color;

struct Light {
    vec3 Position;
    vec3 Color;
};

uniform Light lights[2];

void main()
{
   FragColor = vec4(lights[0].Position - lights[1].Position, 1);
}