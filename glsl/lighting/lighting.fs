// #version 330 core
// out vec4 FragColor;

// in vec3 normal;
// in vec3 frag_positon;

// uniform vec3 light_positon;
// uniform vec3 view_positon;

// void main()
// {
//     vec3 object_color = vec3(1.0, 0.5, 0.31);
//     vec3 light_color = vec3(1.0,1.0,1.0);
//     vec3 light_direction = normalize(light_positon - frag_positon);//光源方向
//     vec3 view_direction = normalize(view_positon - frag_positon);//视线方向
//     vec3 nnormal = normalize(normal);//反射面法向量
//     vec3 half_vector = normalize(light_direction + view_direction);//半程向量

//     // ambient 环境光
//     float ambient_strength = 0.1;
//     vec3 ambient = ambient_strength * light_color;

//     // diffusion 漫反射
//     float diffusion_strength = 0.4 * max(dot(light_direction, nnormal), 0.0);
//     vec3 diffusion = diffusion_strength * light_color;

//     // specular 镜面反射
//     float specular_strength = dot(half_vector, nnormal);
//     if (specular_strength > 0.0)
//     {
//         specular_strength = pow(specular_strength, 64);
//     }
//     else
//     {
//         specular_strength = 0.0;
//     }
//     vec3 specular = specular_strength * light_color;

//     vec3 color = (ambient + diffusion + specular) * object_color;
  	
//     FragColor = vec4(color, 1.0);
// }
#version 330 core
out vec4 FragColor;

in vec3 Normal;  
in vec3 FragPos;  
  
uniform vec3 lightPos; 
uniform vec3 viewPos; 

void main()
{

    vec3 objectColor = vec3(1.0, 0.5, 0.31);
    vec3 lightColor = vec3(1.0);
    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
} 