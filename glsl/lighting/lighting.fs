#version 330 core
out vec4 FragColor;

in vec3 FragPos;  
in vec3 Normal;  
in vec2 TexCoords;

struct Material 
{
    sampler2D diffuse;
    sampler2D specular;    
    float shininess;
}; 

struct Light 
{
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

struct DirectLight 
{
    vec3 direction;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight 
{
    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

uniform vec3 viewPos;
uniform Material material;
uniform DirectLight dirLight;
uniform PointLight pointLights[4];
uniform SpotLight spotLight;

vec3 direct_light(DirectLight light, vec3 normal, vec3 frag_pos, vec3 view_pos)
{
    vec3 light_direction = normalize(-light.direction);
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    // diffuse
    vec3 diffuse = light.diffuse * texture(material.diffuse, TexCoords).rgb;
    float lambert = dot(normal, light_direction);
    diffuse *= max(lambert, 0.0);
    // specular
    vec3 specular = light.specular * texture(material.specular, TexCoords).rgb;
    vec3 view_direction = normalize(view_pos - frag_pos);
    vec3 half_vector = normalize(view_direction + light_direction);
    float blinn_phong = dot(half_vector, normal);
    blinn_phong = blinn_phong > 0.0 ? pow(blinn_phong, material.shininess) : 0.0;
    specular *= blinn_phong;
    // result
    return ambient + diffuse + specular;
}

vec3 point_light(PointLight light, vec3 normal, vec3 frag_pos, vec3 view_pos)
{
    vec3 light_direction = normalize(light.position - FragPos);
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    // diffuse
    vec3 diffuse = light.diffuse * texture(material.diffuse, TexCoords).rgb;
    float lambert = dot(normal, light_direction);
    diffuse *= max(lambert, 0.0);
    // specular
    vec3 specular = light.specular * texture(material.specular, TexCoords).rgb;
    vec3 view_direction = normalize(view_pos - frag_pos);
    vec3 half_vector = normalize(view_direction + light_direction);
    float blinn_phong = dot(half_vector, normal);
    blinn_phong = blinn_phong > 0.0 ? pow(blinn_phong, material.shininess) : 0.0;
    specular *= blinn_phong;
    // attenuation
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / 
    (light.constant + light.linear * distance + light.quadratic * distance * distance);
    // result
    return (ambient + diffuse + specular) * attenuation;
}

vec3 spot_light(SpotLight light,  vec3 normal, vec3 frag_pos, vec3 view_pos)
{
    vec3 light_direction = normalize(light.position - FragPos);
    // ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;
    // diffuse
    vec3 diffuse = light.diffuse * texture(material.diffuse, TexCoords).rgb;
    float lambert = dot(normal, light_direction);
    diffuse *= max(lambert, 0.0);
    // specular
    vec3 specular = light.specular * texture(material.specular, TexCoords).rgb;
    vec3 view_direction = normalize(view_pos - frag_pos);
    vec3 half_vector = normalize(view_direction + light_direction);
    float blinn_phong = dot(half_vector, normal);
    blinn_phong = blinn_phong > 0.0 ? pow(blinn_phong, material.shininess) : 0.0;
    specular *= blinn_phong;
    // attenuation
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / 
    (light.constant + light.linear * distance + light.quadratic * distance * distance);
    ambient *= attenuation; 
    diffuse *= attenuation;
    specular *= attenuation;
    // spotlight with soft edge
    float theta = dot(light_direction, normalize(-light.direction));
    float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;
    // result
    return ambient + diffuse + specular;
}

void main()
{
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    // direct light
    vec3 result = direct_light(dirLight, norm, FragPos, viewDir);
    // point lights
    for(int i = 0; i < 4; i++)
    {
        result += point_light(pointLights[i], norm, FragPos, viewDir);
    }
    // spot light
    result += spot_light(spotLight, norm, FragPos, viewDir);
    // result
    FragColor = vec4(result, 1.0);
}
