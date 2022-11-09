// HLSL code for rendering to texture

Texture2D tex          : register(t0);
SamplerState samLinear : register(s0);

struct RT_VS_INPUT
{
    float4 Position : SV_POSITION;
    float2 UV       : TEXCOORD0;
};

struct RT_PS_INPUT
{
    float4 Position : SV_POSITION;
    float2 UV       : TEXCOORD0;
};


// ----------------
//  VERTEX SHADER
// ----------------
RT_PS_INPUT VS(RT_VS_INPUT input)
{
    RT_PS_INPUT output = (RT_PS_INPUT)0;

    output.Position = input.Position;
    output.UV       = input.UV;

    return output;
}
// ---------------------------------------------------------------------

// ---------------
//  PIXEL SHADER
// ---------------
float4 PS(RT_PS_INPUT input) : SV_TARGET
{
    float4 color = tex.Sample(samLinear, input.UV);
    
    return color;
}
// ---------------------------------------------------------------------
