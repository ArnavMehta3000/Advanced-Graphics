cbuffer WVPBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer LightCameraBuffer : register(b1)
{
    float4 EyePosition;
    float4 LightDiffuse;
}

Texture2D<float4> GDiffuse : register(t0);
Texture2D<float4> GNormal  : register(t1);
Texture2D<float4> GDepth   : register(t2);
SamplerState samLinear     : register(s0);


struct VSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : UV;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : UV;
};

VSOutput VS(VSInput input)
{
    VSOutput output;
    output.Position = input.Position;
    output.TexCoord = input.TexCoord;
    return output;
}

float4 PS(VSOutput input) : SV_Target0
{
    return GDepth.Sample(samLinear, input.TexCoord) /** LightDiffuse*/;
}