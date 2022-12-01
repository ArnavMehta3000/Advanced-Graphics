Texture2D<float4> GDiffuse  : register(t0);
Texture2D<float4> GNormal   : register(t1);
Texture2D<float4> GPosition : register(t2);
SamplerState samLinear      : register(s0);

struct RT_PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 UV       : UV;
};

cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer SceneParams
{
    float3 EyePositionWS;
    float Padding;
}

cbuffer LightParams
{
    float4 PointLightColor;
    float3 PointLightPositionWS;
    float  PointLightRange;
}

float4 PS(RT_PS_INPUT input) : SV_Target3
{
    return float4(input.UV, 0,0);

}