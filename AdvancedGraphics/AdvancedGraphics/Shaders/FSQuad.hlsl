#include "Advanced/Common.hlsli"

Texture2D<float4> render   : register(t0);
Texture2D<float4> velocity : register(t1);
SamplerState samLinear     : register(s0);


cbuffer PostProcessing : register(b0)
{
    bool EnableVignette;
    bool EnableGrayscale;
    bool EnableMotionBlur;
    int _padding0;
    
    int MotionBlurSampleCount;
    float2 VignetteRadiusSoftness;
    
    float _padding1;
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




float4 DoGrayscale(float4 color)
{
    float avg = (color.x + color.y + color.z) / 3.0f;
    return float4(avg, avg, avg, 1.0f);
}

float4 DoVignette(float2 uv)
{
    float len      = distance(uv, float2(0.5, 0.5)) * 0.7f;
    float vignette = smoothstep(VignetteRadiusSoftness.x, VignetteRadiusSoftness.x - VignetteRadiusSoftness.y, len);
    return vignette;
}


float4 DoMotionBlur(float4 color, float2 uv)
{
    float4 result = color;
    float2 vel = velocity.Sample(samLinear, uv).xy;
    
 
    [unroll(50)]
    for (int i = 0; i < MotionBlurSampleCount; i++)
    {
        uv -= abs(vel);
    
        if (!IsUVInBounds(uv))
            return float4(0, 0, 0, 0);
    
        float4 currentColor = render.Sample(samLinear, uv);
        result += currentColor;
    }
    return result / MotionBlurSampleCount;
}

float4 DoPostProcess(float4 color, float2 uv)
{
    if (EnableMotionBlur)
        color = DoMotionBlur(color, uv);
    
    if (EnableGrayscale)        
        color = DoGrayscale(color);
    
    if (EnableVignette)
        color.xyz *= DoVignette(uv).xyz;
    
    
    return color;
}

float4 PS(RT_PS_INPUT input) : SV_TARGET0
{
    float4 color = render.Sample(samLinear, input.UV);
    return DoPostProcess(color, input.UV);
}