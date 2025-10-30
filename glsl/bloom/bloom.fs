#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

struct Light {
    vec3 Position;
    vec3 Color;
};
uniform Light lights[2];

uniform sampler2D diffuseTexture;
uniform vec3 viewPos;
// uniform Light lights;


void main()
{
    FragColor = vec4(lights[0].Position - lights[1].Position, 1);
    return;
    // vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb;
    // vec3 normal = normalize(fs_in.Normal);
    // // ambient
    // vec3 ambient = 0.0 * color;
    // // lighting
    // vec3 lighting = vec3(0.0);
    // vec3 viewDir = normalize(viewPos - fs_in.FragPos);

    // // diffuse
    // vec3 lightDir = normalize(lights.Position - fs_in.FragPos);
    // float diff = max(dot(lightDir, normal), 0.0);
    // vec3 result = lights.Color * diff * color;      
    // // attenuation (use quadratic as we have gamma correction)
    // float distance = length(fs_in.FragPos - lights.Position);
    // result *= 1.0 / (distance * distance);
    // lighting += result;

    
    // FragColor = vec4(lighting + ambient, 1.0);
}