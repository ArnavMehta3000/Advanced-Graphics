Texture2D<float4> renderTarget : register(t0);
SamplerState samLinear         : register(s0);


cbuffer PostProcessing : register(b0)
{
    matrix CurrentViewProjection;
    matrix PrevViewProjection;
    bool EnableVignette;
    bool EnableGrayscale;
    float2 VignetteRadiusSoftness;
}

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





//float4 GetProjectionPos(float2 uv, matrix proj)
//{
//    float4 pos = float4(GDepth.Sample(samLinear, uv).xyz, 1.0f);
//    return mul(pos, mul(View, proj));
//}

float4 Grayscale(float4 color)
{
    float avg = (color.x + color.y + color.z) / 3.0f;
    return float4(avg, avg, avg, 1.0f);
}


//float2 GetPixelVelocity(float2 uv)
//{
//    float4 currentPos = GetProjectionPos(uv, CurrentViewProjection);
//    float4 prevPos = GetProjectionPos(uv, PrevViewProjection);

//    return ((currentPos - prevPos) / 2.0f).xy;
//}

float4 DoVignette(float2 uv)
{
    float len = distance(uv, float2(0.5, 0.5)) * 0.7f;
    float vignette = smoothstep(VignetteRadiusSoftness.x, VignetteRadiusSoftness.x - VignetteRadiusSoftness.y, len);
    return vignette;
}

float4 DoPostProcess(float4 color, float2 uv)
{
    if (EnableGrayscale)        
        color = Grayscale(color);
    
    if (EnableVignette)
        color.xyz *= DoVignette(uv).xyz;
    
    
    return color;
}

float4 PS(RT_PS_INPUT input) : SV_TARGET0
{
    float4 color = renderTarget.Sample(samLinear, input.UV);
    return DoPostProcess(color, input.UV);
}