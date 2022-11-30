Texture2D<float4> GDiffuse  : register(t0);
Texture2D<float4> GNormal   : register(t1);
Texture2D<float4> GPosition : register(t2);

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

float4 PS() : SV_Target0
{
    return float4(1, 1, 0, 1);
}