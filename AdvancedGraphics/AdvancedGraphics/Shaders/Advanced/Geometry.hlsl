cbuffer WVPBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

// For PS only
cbuffer SurfaceProperties : register(b1)
{
    float3 SpecularColor;
    float SpecularPower;
}

Texture2D<float4> DiffuseMap : register(t0);
Texture2D<float4> NormalMap  : register(t1);
Texture2D<float4> HeightMap  : register(t2);
SamplerState samLinear       : register(s0);
// ~For PS only~


struct VSInput
{
    float3 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float3 Tangent  : TANGENT;
    float3 Binormal : BINORMAL;
};

struct PSInput
{
    float4 Position   : SV_POSITION;
    float2 TexCoord   : TEXCOORD0;
    float3 NormalWS   : NORMALWS;
    float3 PositionWS : POSITIONWS;
    float3 TangentWS  : TANGENTWS;
    float3 BinormalWS : BINORMALWS;
    float3x3 TBN      : TBN;
};

// For PS only
struct PSOutput
{
    float4 DiffuseAlbedo : SV_TARGET0;
    float4 Normal        : SV_TARGET1;
    float4 Depth         : SV_Target2;
};
// ~For PS only~



PSInput VS(VSInput input)
{
    PSInput output;

    float4 pos = float4(input.Position, 1.0f);

    output.PositionWS = mul(pos, World).xyz;
    output.NormalWS = normalize(mul(input.Normal, (float3x3) World));

    pos = float4(output.PositionWS, 1.0f);
    output.Position = mul(pos, View);
    output.Position = mul(output.Position, Projection);

    output.TangentWS = normalize(mul(float4(input.Tangent, 0.0f), World)).xyz;
    output.BinormalWS = normalize(mul(float4(input.Binormal, 0.0f), World)).xyz;
    output.TexCoord = input.TexCoord;

    output.TBN = float3x3(output.TangentWS, output.BinormalWS, output.NormalWS);
    return output;
}



PSOutput PS(PSInput input)
{
    PSOutput output;

    float3 diffuseColor  = DiffuseMap.Sample(samLinear, input.TexCoord).rgb;
    float heightDepth    = HeightMap.Sample(samLinear, input.TexCoord).r;
    float4 normalTS      = NormalMap.Sample(samLinear, input.TexCoord);

    float4 normalWS      = float4(mul(normalTS.xyz, input.TBN), 1.0f);
    
    output.DiffuseAlbedo = float4(diffuseColor, heightDepth);
    output.Normal        = normalWS;
    float depth          = input.Position.z / input.Position.w;
    output.Depth         = float4(input.PositionWS, depth);

    return output;
}