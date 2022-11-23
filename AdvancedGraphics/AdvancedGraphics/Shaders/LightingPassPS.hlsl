#include "Structures.hlsli"

Texture2D<float4> GDiffuse : register (t0);
Texture2D<float4> GNormal  : register (t1);
Texture2D<float4> GPosition: register (t2);


float4 PS() : SV_TARGET0
{
    float4 finalColor = float4(1, 1, 0, 1);


    return finalColor;
}