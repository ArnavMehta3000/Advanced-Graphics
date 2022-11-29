#include "Structures.hlsli"

Texture2D<float4> GDiffuse : register (t0);
Texture2D<float4> GNormal  : register (t1);
Texture2D<float4> GPosition: register (t2);

struct SurfaceData
{
    float LinearDepth;
    float4 Color;
    float4 Normal;
};


SurfaceData UnpackGBuffer()
{
    SurfaceData output = (SurfaceData) 0;

    output.LinearDepth = 1.0f;
    output.Color = float4(0,1,0,1);
    output.Normal = float4(1, 1, 0,1);

    return output;
}



float4 PS() : SV_TARGET0
{
    return float4(1, 0,0, 1);
}