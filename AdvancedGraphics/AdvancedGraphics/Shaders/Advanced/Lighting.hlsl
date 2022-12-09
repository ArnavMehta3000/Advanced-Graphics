#include "Common.hlsli"

cbuffer WVPBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer LightCameraBuffer : register(b1)
{
    matrix InvView;
    matrix InvProjection;
    float4 EyePosition;
    float4 GlobalAmbient;
    Light PointLight;
}

Texture2D<float4> GDiffuse  : register(t0);
Texture2D<float4> GNormal   : register(t1);
Texture2D<float4> GDepth : register(t2);
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
    output.Position = input.Position;
    output.TexCoord = input.TexCoord;
    output.PositionWS = mul(input.Position, mul(mul(World, View), Projection));
    return output;
}

float4 GetGNormals(float2 uv)
{
    return GNormal.Sample(samLinear, uv);
}

float4 GetGDepth(float2 uv)
{
    float depth = GDepth.Sample(samLinear, uv).r;
    //depth = depth * 2.0f - 1.0f;
    float near = 0.01f;
    float far = 100.0f;
    float linearDepth = Projection._43 / (depth - Projection._33);
    
    return linearDepth;
}

float4 GetGDiffuse(float2 uv)
{
    return GDiffuse.Sample(samLinear, uv);
}


float2 CalculateParallaxOffset(float2 uv, float3 viewDir, float3 normal)
{
    viewDir = normalize(viewDir);
    // Compute all the derivatives
    float2 dx = ddx(uv);
    float2 dy = ddy(uv);

    float minLayers   = PointLight.Parallax.x;
    float maxLayers   = PointLight.Parallax.y;
    float heightScale = PointLight.Parallax.z;
    float2 bias       = PointLight.Bias.xy;

    float numLayers         = lerp(maxLayers, minLayers, abs(dot(viewDir, normal)));
    float layerDepth        = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    float2 offset = (viewDir.xy / viewDir.z * heightScale) + bias;
    float2 deltaTexCoords = offset / numLayers;

    float2 currentTexCoords    = uv;
    float currentDepthMapValue = GDiffuse.SampleGrad(samLinear, currentTexCoords, dx, dy).a;

    while (currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = GDiffuse.SampleGrad(samLinear, currentTexCoords, dx, dy).a;
        currentLayerDepth += layerDepth;
    }

    float2 prevTexCoords = currentTexCoords + deltaTexCoords;

    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = GDiffuse.SampleGrad(samLinear, currentTexCoords, dx, dy).a - currentLayerDepth + layerDepth;

    float weight          = afterDepth / (afterDepth - beforeDepth);
    float2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
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
    
    LightingResult lighting = ComputeLighting(GetGDepth(texCoord).xyz, GetGNormals(texCoord).xyz);
    
    float3 finalColor = (GlobalAmbient + lighting.Diffuse + lighting.Specular).xyz * GetGDiffuse(texCoord).xyz;
    return GetGDepth(input.TexCoord);
}