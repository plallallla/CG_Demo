#version 400 core

layout (location = 0) out vec4 out_color;

const float PI = 3.14159265359;

struct PointLight
{
    vec3 position;
    vec3 color;    
};

// 调试用 实际应该从对应贴图采样
uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ao;

uniform vec3 view_pos;
uniform float ambient_intensity;

uniform PointLight light_array[4];

in vec3 vsout_frag_pos;
in vec3 vsout_normal;
in vec2 vsout_uv;

vec3 calculate_radiance(vec3 light_pos, vec3 frag_pos, vec3 light_color)
{
    float d = max(length(light_pos - frag_pos), 1e-4);
    return light_color / (d * d);
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)//F0基准反射率 theta在点光源时为dot(H, V)，在方向光源时为dot(N, V)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

// vec3 FresnelSchlick(float cosTheta, vec3 F0, float roughness)
// {
//     return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
// }

float GeometrySmithGGX(float NdotL, float NdotV, float roughness)
{
    float NdotL2 = NdotL * NdotL;
    float NdotV2 = NdotV * NdotV;
    float alpha2 = roughness * roughness + 0.0001f;
    float G_L = (2.0 * NdotL) / (NdotL + sqrt(NdotL2 + alpha2 * (1.0 - NdotL2)));
    float G_V = (2.0 * NdotV) / (NdotV + sqrt(NdotV2 + alpha2 * (1.0 - NdotV2)));
    return G_L * G_V;
}

float NDFGGX(vec3 H, vec3 N, float roughness)
{
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float denom = NdotH2 * (alpha2 - 1.0) + 1.0;
    return alpha2 / (PI * denom * denom);
}

float Fd90(float dot_val, float roughness)
{
    return (2.0 * dot_val * roughness) + 0.4f;
}

float KDisneyTerm(float NdotL, float NdotV, float roughness)
{
    float L = (1.0 + Fd90(NdotL, roughness) * pow(1.0 - NdotL, 5.0));
    float V = (1.0 + Fd90(NdotV, roughness) * pow(1.0 - NdotV, 5.0));
    return L * V;
}

void main()
{
    vec3 N = normalize(vsout_normal);
    vec3 V = normalize(view_pos - vsout_frag_pos);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < 4; i++)
    {
        vec3 light_pos = light_array[i].position;
        vec3 light_color = light_array[i].color;
        vec3 L = normalize(light_pos - vsout_frag_pos);
        vec3 H = normalize(L + V);
        vec3 radiance = calculate_radiance(light_pos, vsout_frag_pos, light_color);

        float NdotL = clamp(dot(N, L), 0.0, 1.0);
        float NdotV = clamp(dot(N, V), 0.0, 1.0);

        // specular
        vec3 F = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
        float D = NDFGGX(H, N, roughness);
        float G = GeometrySmithGGX(NdotL, NdotV, roughness);
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 1e-4;
        vec3 specular = (F * G * D) / denominator;

        // diffuse
        vec3 diffuse = albedo * (1.0 - metallic) / PI;
        diffuse *= KDisneyTerm(NdotL, NdotV, roughness);

        Lo += (diffuse + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(ambient_intensity) * albedo * ao;

    vec3 color = ambient + Lo;

    color = color / (color + vec3(1.0)); // hdr
    color = pow(color, vec3(1.0/2.2)); // gamma

    out_color = vec4(color.rgb, 1.0);
}