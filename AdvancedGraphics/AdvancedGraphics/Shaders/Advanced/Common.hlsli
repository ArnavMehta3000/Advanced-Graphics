#define MAX_LIGHTS 1

struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};

struct Light
{
    float4 Position;
    
    float3 Diffuse;
    float Intensity;
    
    float3 Specular;
    float Radius;
    
    float4 Parallax;
    float4 Bias;
    
    float SpecularPower;
    float3 _padding0;
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
    return float4(light.Diffuse * diffuse.xyz, 1.0f);
}

float4 DoPointLightSpecular(Light light, float3 lightDir, float3 eyeDir, float3 vertPos, float3 vertNormal, float surfaceSpec)
{
    float3 reflectDir = reflect(-lightDir, vertNormal);
    float specular    = pow(max(dot(eyeDir, reflectDir), 0.0f), surfaceSpec);
    return float4(specular * light.Specular, 1.0f);
}

float4 WorldPosFromDepth(float2 uv, float depth, matrix invProj)
{
    // Convert uv to ndc
    float x = uv.x * 2.0f - 1.0f;
    float y = (1.0f - uv.y) * 2.0f - 1.0f;
    float z = depth;
    
    float4 projectedPos = float4(x, y, z, 1.0f);
    float4 positionVS = mul(projectedPos, invProj);
    return float4(positionVS.xyz / positionVS.w, 1.0f);
}