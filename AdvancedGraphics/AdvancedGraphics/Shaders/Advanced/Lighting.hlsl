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

struct SurfaceData
{
    float Depth;
    float4 Position;
    float4 Diffuse;
    float4 Normal;
};



VSOutput VS(VSInput input)
{
    VSOutput output;
    output.Position = input.Position;
    output.TexCoord = input.TexCoord;
    return output;
}

float4 GetPositionFromDepth(float depth)
{
    return float4(1, 1, 1, 1);
}

SurfaceData UnpackGBuffer(float2 uv)
{
    SurfaceData output;
    output.Depth    = GDepth.Sample(samLinear, uv).r;
    output.Position = GetPositionFromDepth(output.Depth);
    output.Diffuse  = GDiffuse.Sample(samLinear, uv);
    output.Normal   = GNormal.Sample(samLinear, uv);
    
    return output;
}


float4 PS(VSOutput input) : SV_Target0
{
    return UnpackGBuffer(input.TexCoord).Depth * LightDiffuse;
}