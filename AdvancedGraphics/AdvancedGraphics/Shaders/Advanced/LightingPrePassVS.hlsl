struct RT_VS_INPUT
{
    float4 Position : SV_POSITION;
    float2 UV       : UV;
};

struct RT_PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 UV       : UV;
};

RT_PS_INPUT VS(RT_VS_INPUT input)
{
    RT_PS_INPUT output = (RT_PS_INPUT) 0;

    output.Position = input.Position;
    output.UV = input.UV;

    return output;
}