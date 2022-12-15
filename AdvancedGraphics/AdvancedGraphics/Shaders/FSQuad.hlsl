Texture2D<float4> renderTarget : register(t0);
SamplerState samLinear         : register(s0);

struct RT_VS_INPUT
{
    float4 Position : SV_POSITION;
    float2 UV : UV;
};

struct RT_PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 UV : UV;
};

RT_PS_INPUT VS(RT_VS_INPUT input)
{
    RT_PS_INPUT output = (RT_PS_INPUT) 0;

    output.Position = input.Position;
    output.UV = input.UV;

    return output;
}

float4 PS(RT_PS_INPUT input) : SV_TARGET0
{
    return renderTarget.Sample(samLinear, input.UV) * 100;
}