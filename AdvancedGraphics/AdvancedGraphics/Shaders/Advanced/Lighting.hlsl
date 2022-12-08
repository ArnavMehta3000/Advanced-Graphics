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
    Light Lights[MAX_LIGHTS];
}

Texture2D<float4> GDiffuse  : register(t0);
Texture2D<float4> GNormal   : register(t1);
Texture2D<float4> GPosition : register(t2);
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
struct SurfaceData
{
    //float Depth;
    float4 Position;
    float4 Diffuse;
    float4 Normal;
};



VSOutput VS(VSInput input)
{
    VSOutput output;
    output.Position = input.Position;
    output.TexCoord = input.TexCoord;
    output.PositionWS = mul(input.Position, mul(mul(World, View), Projection));
    return output;
}


SurfaceData UnpackGBuffer(VSOutput input)
{
    int3 pos = int3((int2) input.Position.xy, 0);
    
    SurfaceData output;
    output.Position = GPosition.Load(pos);
    output.Diffuse  = GDiffuse.Load(pos);
    output.Normal   = GNormal.Load(pos);
    output.Normal   = normalize(2.0f * output.Normal - 1.0f);  // Convert from [0,1] to [-1, 1]
    
    return output;
}

LightingResult ComputeLighting(SurfaceData input)
{
    LightingResult result = (LightingResult) 0;
    
    [unroll(MAX_LIGHTS)]
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        Light l = Lights[i];
        
        float3 lightDir     = normalize(l.Position.xyz - input.Position.xyz);
        float3 viewDir      = normalize(EyePosition - input.Position).xyz;
        float distFromLight = length(l.Position.xyz - input.Position.xyz);
        
        // Done in world space
        result.Diffuse += DoPointLightDiffuse(l, lightDir, viewDir, input.Position.xyz, input.Normal.xyz);
        result.Specular += DoPointLightSpecular(l, lightDir, viewDir, input.Position.xyz, input.Normal.xyz, 32);
        
        float attenuation = DoAttenutation(distFromLight, l.Attenuation.zyx);
        
        result.Diffuse *= attenuation;
        result.Specular *= attenuation;
    }
    
    return result;
}

float4 PS(VSOutput input) : SV_Target0
{    
    SurfaceData surfaceData = UnpackGBuffer(input);
    LightingResult lighting = ComputeLighting(surfaceData);
    
    float3 finalColor = (GlobalAmbient + lighting.Diffuse + lighting.Specular).xyz * surfaceData.Diffuse.xyz;
    return float4(finalColor, 1.0f);
}