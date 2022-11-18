// HLSL code for rendering to texture

Texture2D<float4> renderTarget : register(t0);
SamplerState samLinear : register(s0);

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

// -----------
//  FUNCTIONS
// -----------
float4 Blur(float2 uv)
{
    const float4 color = renderTarget.Sample(samLinear, uv);
    
    return color;
}

float4 Grayscale(float2 uv)
{
    float4 color = renderTarget.Sample(samLinear, uv);
    float avg = (color.x + color.y + color.z) / 3.0f;

    return float4(avg, avg, avg, 1.0f);
}

float4 Vignette(float2 uv)
{
    const float2 resolution = float2(1280, 720);
    const float4 color = renderTarget.Sample(samLinear, uv);
    
    float2 texCoord = uv.xy / resolution.xy;
    texCoord *= 1.0f - texCoord.xy;
    
    float vignette = texCoord.x * texCoord.y * 15.0f;
    vignette = pow(vignette, 0.5f);
    return float4(color * vignette);
}
// ---------------------------------------------------------------------

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
float4 PS(RT_PS_INPUT input) : SV_TARGET0
{
    float4 color = renderTarget.Sample(samLinear, input.UV);
    return Grayscale(input.UV);
    //return float4(1.0f, 0.0f, 0.0f, 1.0f);
}
// ---------------------------------------------------------------------
