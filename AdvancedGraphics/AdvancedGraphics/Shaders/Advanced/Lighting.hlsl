#include "Common.hlsli"

cbuffer WVPBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer LightCameraBuffer : register(b1)
{
    matrix CurrentViewProjection;
    matrix PrevViewProjection;
    float4 EyePosition;
    float4 GlobalAmbient;
    Light PointLight;
}

cbuffer PostProcessing : register(b2)
{
    bool EnableVignette;
    bool EnableGrayscale;
    float2 VignetteRadiusSoftness;
}

Texture2D<float4> GDiffuse  : register(t0);
Texture2D<float4> GNormal   : register(t1);
Texture2D<float4> GDepth    : register(t2);
SamplerState samLinear      : register(s0);


struct VSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : UV;
};

struct VSOutput
{
    float4 Position   : SV_POSITION;
    float4 PositionWS : POSITIONWS;
    float2 TexCoord   : UV;
};



VSOutput VS(VSInput input)
{
    VSOutput output;
    output.Position   = input.Position;
    output.TexCoord   = input.TexCoord;
    output.PositionWS = mul(input.Position, mul(View, Projection));
    return output;
}

float4 GetGNormals(float2 uv)
{
    return GNormal.Sample(samLinear, uv);
}


float4 GetGPosDepth(float2 uv)
{
    float4 pos        = GDepth.Sample(samLinear, uv);
    float depth       = pos.a;
        
    float near        = 0.01f;
    float far         = 100.0f;
    float projectionA = far / far - near;
    float projectionB = (-far * near) / (far - near);
    float linearDepth = projectionA / (depth - projectionB);

    // world position-depth (RGB-A)
    return float4(pos.xyz, linearDepth / 100.0f);
    
    // Ref: https://developer.nvidia.com/gpugems/gpugems3/part-iv-image-effects/chapter-27-motion-blur-post-processing-effect
}

float4 GetGDiffuse(float2 uv)
{
    return GDiffuse.Sample(samLinear, uv);
}

float4 GetProjectionPos(float2 uv, matrix proj)
{
    float4 pos = float4(GDepth.Sample(samLinear, uv).xyz, 1.0f);
    return mul(pos, mul(View, proj));
}

float2 GetPixelVelocity(float2 uv)
{
    float4 currentPos = GetProjectionPos(uv, CurrentViewProjection);
    float4 prevPos    = GetProjectionPos(uv, PrevViewProjection);

    return ((currentPos - prevPos) / 2.0f).xy;
}



LightingResult ComputeLighting(float3 position, float3 normal)
{
    LightingResult result = (LightingResult) 0;
    
    float3 lightDir     = normalize(PointLight.Position.xyz - position);
    float3 viewDir      = normalize(EyePosition.xyz - position).xyz;
    float3 distFromLight = PointLight.Position.xyz - position;
    
    // Done in world space
    result.Diffuse += DoPointLightDiffuse(PointLight, lightDir, viewDir, position, normal);
    result.Specular += DoPointLightSpecular(PointLight, lightDir, viewDir, position, normal, PointLight.SpecularPower);
    
    float atten = lerp(PointLight.Intensity, 0.0f, min(length(distFromLight), PointLight.Radius) / PointLight.Radius);
    result.Diffuse *= atten;
    result.Specular*= atten;
    
    return result;
}

float4 DoVignette(float2 uv)
{
    float len = distance(uv, float2(0.5, 0.5)) * 0.7f;
    float vignette = smoothstep(VignetteRadiusSoftness.x, VignetteRadiusSoftness.x - VignetteRadiusSoftness.y, len);
    return vignette;
}

float4 Grayscale(float4 color)
{
    float avg = (color.x + color.y + color.z) / 3.0f;
    return float4(avg, avg, avg, 1.0f);
}

float4 DoPostProcess(float4 color, float2 uv)
{
    if (EnableGrayscale)        
        color = Grayscale(color);
    
    if (EnableVignette)
        color.xyz *= DoVignette(uv).xyz;
    
    
    return color;
}

float4 PS(VSOutput input) : SV_Target0
{
    float3 lightDir = normalize(PointLight.Position.xyz - input.Position.xyz);
    float3 viewDir = normalize(EyePosition - input.Position).xyz;
    
    float2 texCoord = input.TexCoord;
    //CalculateParallaxOffset(input.TexCoord, viewDir, GetGNormals(input.TexCoord).xyz);
    if (!IsUVInBounds(texCoord))
        return float4(1, 0, 0, 1);
    
    // Convert from [0,1] to [-1, 1]
    float3 normals = normalize(2.0f * GetGNormals(texCoord).xyz - 1.0f);
    
    LightingResult lighting = ComputeLighting(GetGPosDepth(texCoord).xyz, GetGNormals(texCoord).xyz);
    
    float4 finalColor = (GlobalAmbient + lighting.Diffuse + lighting.Specular) * GetGDiffuse(texCoord);
     
    float2 velocity = GetPixelVelocity(texCoord);
    
    return DoPostProcess(finalColor, texCoord);
}