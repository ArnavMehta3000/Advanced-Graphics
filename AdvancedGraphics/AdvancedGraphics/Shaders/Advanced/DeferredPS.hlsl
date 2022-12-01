// https://mynameismjp.wordpress.com/2009/03/10/reconstructing-position-from-depth/
// https://github.com/ruchitak3030/Deferred-Rendering-DirectX11/

cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer SurfaceProperties : register(b1)
{
    float3 SpecularColor;
    float SpecularPower;
}

Texture2D<float4> DiffuseMap : register(t0);
Texture2D<float4> NormalMap  : register(t1);
Texture2D<float4> HeightMap  : register(t2);
Texture2D<float4> DepthMap   : register(t2);
SamplerState      samLinear  : register(s0);


struct VSOutput
{
    float4 Position   : SV_POSITION;
    float2 TexCoord   : TEXCOORD0;
    float3 NormalWS   : NORMALWS;
    float3 PositionWS : POSITIONWS;
    float3 TangentWS  : TANGENTWS;
    float3 BinormalWS : BINORMALWS;
    float3x3 TBN      : TBN;
};

struct PSOutput
{
    float4 DiffuseAlbedo : SV_TARGET0;
    float4 Normal        : SV_TARGET1;
    float4 Depth         : SV_Target2;
    //float4 Acc : SV_Target3;
};

PSOutput PS(VSOutput input)
{
    PSOutput output;

    float3 diffuseColor = DiffuseMap.Sample(samLinear, input.TexCoord).rgb;
    float heightDepth   = HeightMap.Sample(samLinear, input.TexCoord).r;
    float4 normalTS     = NormalMap.Sample(samLinear, input.TexCoord);
    normalTS            = float4(normalize(2.0f * normalTS.xyz - 1.0f).xyz, 1.0f);

    float4 normalWS = float4(mul(normalTS.xyz, input.TBN), 1.0f);

    output.DiffuseAlbedo = float4(diffuseColor, heightDepth);
    output.Normal        = float4(input.NormalWS, 1.0f);
    output.Depth         = float4(input.PositionWS, 1.0f); //DepthMap.Sample(samLinear, input.TexCoord);

    //output.Acc = lerp(output.DiffuseAlbedo, output.Depth, 0.5f);

    return output;
}