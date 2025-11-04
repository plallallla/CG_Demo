//基础着色 应用纹理
#version 330 core

layout (location = 0) out vec4 frag_color;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 UV;
} vs_out;


vec3 ambient(vec3 color)
{
    return color;
}

vec3 diffuse(vec3 color, vec3 normal, vec3 light_direction)
{
    return color * max(dot(normal, light_direction), 0.0);
}

vec3 specular(vec3 color, float shininess, vec3 normal, vec3 light_direction, vec3 view_direction)
{
    vec3  half_vector = normalize(light_direction + view_direction);
    float specular_value = max(dot(half_vector, normal), 0.0);
    specular_value = pow(specular_value, shininess);
    return color * specular_value;
}

struct Material
{
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
uniform Material material;

struct Light
{
    vec3 position;
    vec3 color;
};
uniform Light light;

uniform vec3 view_pos;

void main()
{
    vec3 ambient_color = texture(material.ambient, vs_out.UV).rgb;
    ambient_color = 0.1 * ambient(ambient_color);

    vec3 normal = normalize(vs_out.Normal);
    vec3 light_dir = normalize(light.position - vs_out.FragPos);
    vec3 view_dir = normalize(view_pos - vs_out.FragPos);

    vec3 diffuse_color = light.color * texture(material.diffuse, vs_out.UV).rgb;
    diffuse_color = diffuse(diffuse_color, normal, light_dir);

    vec3 specular_color = light.color * texture(material.specular, vs_out.UV).rgb;
    specular_color = specular(specular_color, material.shininess, normal, light_dir, view_dir);

    frag_color = vec4(ambient_color * 0.05 + diffuse_color * 0.4 + specular_color * 0.5, 1.0);

}