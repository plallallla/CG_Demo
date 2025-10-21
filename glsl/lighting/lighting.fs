#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct Light {
    vec3 position;  
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;
  
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    // diffuse
    vec3 light_dir = normalize(light.position - FragPos);
    vec3 norm = normalize(Normal);
    vec3 diffuse = dot(light_dir, norm) * light.diffuse * texture(material.diffuse, TexCoords).rgb;

    // specular
    vec3 view_dir = normalize(viewPos - FragPos);
    vec3 half_vec = normalize(light_dir + view_dir);
    float spec_strength = dot(half_vec, norm);
    spec_strength = spec_strength > 0 ? pow(spec_strength, 128) : 0;
    vec3 specular = spec_strength * light.specular * texture(material.specular, TexCoords).rgb;

    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * distance * distance);
    ambient *= attenuation; 
    diffuse *= attenuation;
    specular *= attenuation;

    // spotlight with soft edge
    float theta = dot(light_dir, normalize(-light.direction));
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;
    
    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
