#include "Structures.hlsli"

Texture2D<float4> GDiffuse : register (t0);
Texture2D<float4> GNormal  : register (t1);
Texture2D<float4> GPosition: register (t2);


float4 PS()
{
    float4 finalColor = float4(0, 0, 0, 1);


    return finalColor
}