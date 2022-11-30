cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

struct VSInput
{
    float3 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 TexCoord : TEXCOORD0;
    float3 Tangent  : TANGENT;
    float3 Binormal : BINORMAL;
};

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


VSOutput VS(VSInput input)
{
    VSOutput output;

    float4 pos = float4(input.Position, 1.0f);

    output.PositionWS = mul(pos, World).xyz;
    output.NormalWS = normalize(mul(input.Normal, (float3x3)World));

    pos = float4(output.PositionWS, 1.0f);
    output.Position = mul(pos, View);
    output.Position = mul(output.Position, Projection);

    output.TangentWS  = normalize(mul(float4(input.Tangent, 0.0f), World)).xyz;
    output.BinormalWS = normalize(mul(float4(input.Binormal, 0.0f), World)).xyz;
    output.TexCoord   = input.TexCoord;

    output.TBN = float3x3(output.TangentWS, output.BinormalWS, output.NormalWS);

    return output;
}