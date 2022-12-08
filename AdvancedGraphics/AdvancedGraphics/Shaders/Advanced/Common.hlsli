#define MAX_LIGHTS 1

struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};

struct Light
{
    float4 Position;
    float4 Diffuse;
    float4 Specular;
    float4 Attenuation;
};

bool IsUVInBounds(float2 uv)
{
    if (uv.x > 1.0f || uv.y > 1.0f || uv.x < 0.0f || uv.y < 0.0f)
        return false;
    else
        return true;
}

float DoAttenutation(float distFromLight, float3 att)
{
    return 1.0f / (att.x + (att.y * distFromLight) + (att.z * distFromLight * distFromLight));
}

float4 DoPointLightDiffuse(Light light, float3 lightDir, float3 eyeDir, float3 vertPos, float3 vertNormal)
{
    float4 diffuse = max(dot(lightDir, vertNormal), 0.0f);    
    return light.Diffuse * diffuse;
}

float4 DoPointLightSpecular(Light light, float3 lightDir, float3 eyeDir, float3 vertPos, float3 vertNormal, float surfaceSpec)
{
    float3 reflectDir = reflect(-lightDir, vertNormal);
    float specular    = pow(max(dot(eyeDir, reflectDir), 0.0f), surfaceSpec);
    return specular * light.Specular;
}