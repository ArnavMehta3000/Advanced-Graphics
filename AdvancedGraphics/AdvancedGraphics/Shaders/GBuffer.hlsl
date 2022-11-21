// Vertex/Pixel HLSL shader for GBuffer Rendering

// GBuffer texture maps
Texture2D<float4> AlbedoMap  : register(t0);
Texture2D<float4> NormalMap  : register(t1);
Texture2D<float4> HeightMap  : register(t2);

SamplerState Sampler : register(s0);


cbuffer GBufferCBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal   : NORMAL; 
    float2 TexCoord : TEXCOORD0;
    float3 Tangent  : TANGENT;
    float3 Binormal : BINORMAL;
};

struct VS_OUTPUT
{
    float4 Position   : SV_POSITION;
    float4 PositionWS : POSITION;
    float3 Normal     : NORMAL;
    float2 TexCoord   : TEXCOORD0;
    float3 Tangent    : TANGENT;
};

struct PS_OUTPUT
{
    float4 Color   : SV_Target0;
    float4 Normal  : SV_Target1;
    float4 Position: SV_TARGET2;
};



// ----- VERTEX SHADER -----
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;

    float4 pos      = float4(input.Position.xyz, 1.0f);
    matrix viewProj = mul(View, Projection);

    output.PositionWS = mul(pos, World);
    output.Position   = mul(output.PositionWS, viewProj);
    output.Normal     = input.Normal;
    output.TexCoord   = input.TexCoord;
    output.Tangent    = input.Tangent;

     // Create the TBN matrix
    float3 T          = normalize(mul(float4(input.Tangent,  0.0f), World)).xyz;
    float3 N          = normalize(mul(float4(input.Normal, 0), World).xyz);
    float3 B          = normalize(mul(float4(input.Binormal, 0.0f), World)).xyz;
    
    float3x3 TBN      = float3x3(T, B, N);
    float3x3 invTBN   = transpose(TBN);

    return output;
}
// ----- VERTEX SHADER -----




// ----- PIXEL SHADER -----
PS_OUTPUT PS(VS_OUTPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0;

    // Get diffuse map
    output.Color = AlbedoMap.Sample(Sampler, input.TexCoord);

    // Get normal map
    float4 texNormal = NormalMap.Sample(Sampler, input.TexCoord);
    output.Normal    = float4(normalize(2.0f * texNormal.xyz - 1.0f).xyz, 1.0f);

    // Get world position map
    output.Position = input.PositionWS;

    return output;
}
// ----- PIXEL SHADER -----
