#include "Structures.hlsli"

Texture2D txDiffuse    : register(t0);
Texture2D txNormal     : register(t1);
Texture2D txHeight     : register(t2);
SamplerState samLinear : register(s0);

cbuffer MaterialProperties : register(b1)
{
    _Material Material;
};

struct VS_OUTPUT
{
    float4 Position  : SV_POSITION;
    float4 PositionW : POSITION;
    float3 PositionT : POSITIONT;
    float2 UV        : TEXCOORD0;
    float3 NormalT   : NORMALT;
    float3 NormalW   : NORMALW;
    float3 LightDirT : TLIGHTDIR;
    float3 EyeDirT   : TEYEDIR;
    float3 EyePosT   : EYEPOSITIONT;
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
        float4 texNormal   = txNormal.Sample(samLinear, input.UV);
        float4 bumpNormalT = float4(normalize(2.0f * texNormal.xyz - 1.0f).xyz, 1.0f);
        output.Normal      = bumpNormalT;
    }
    else
        output.Normal = float4(input.NormalT.xyz, 1.0f);
        
    output.Position = input.PositionW;
    
    return output;
}