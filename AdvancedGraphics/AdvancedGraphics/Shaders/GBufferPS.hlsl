#include "Structures.hlsli"

Texture2D<float4> txDiffuse : register(t0);
Texture2D<float4> txNormal  : register(t1);
Texture2D<float4> txHeight  : register(t2);
SamplerState samLinear      : register(s0);

cbuffer MaterialProperties : register(b1)
{
    _Material Material;
};

cbuffer LightProperties : register(b2)
{
    float4 EyePosition;                // 16 bytes
    float4 GlobalAmbient = (float4) 0; // 16 bytes
    PointLight Light;                  // 48 bytes
}; 

struct PS_OUTPUT
{
    float4 Diffuse  : SV_Target0;
    float4 Normal   : SV_Target1;
    float4 Position : SV_Target2;
};


PS_OUTPUT PS(VS_OUTPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0;

    [branch]
    if (Material.UseTexture)
        output.Diffuse = txDiffuse.Sample(samLinear, input.UV);
    else
        output.Diffuse = Material.Diffuse;  // White color 
    
    [branch]
    if (Material.UseNormals)
    {
        float4 texNormal  = txNormal.Sample(samLinear, input.UV);
        float4 bumpNormal = float4(normalize(2.0f * texNormal.xyz - 1.0f).xyz, 1.0f);  // Tangent space
        output.Normal     = float4(mul(bumpNormal.xyz, transpose(input.TBN)), 1.0f);
    }
    else
        output.Normal = float4(input.NormalW.xyz, 1.0f);
        
    output.Position = input.PositionW;
    
    return output;
}