// ------------
//  STRUCTURES
// ------------
//struct VS_IN  // For triangle
//{
//    float3 Position : POSITION;
//};

struct VS_IN
{
    float3 Position : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;
};

struct VS_OUT
{
    float4 Position : SV_POSITION;
    float4 Col : COLOR;
};


// ------------------
//  CONSTANT BUFFERS
// ------------------
cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
    float4 vOutputColor;
}



// ----------------
//  VERTEX SHADER
// ----------------
VS_OUT VS(VS_IN input)
{
    VS_OUT output = (VS_OUT)0;
    
    float4 pos = float4(input.Position, 1.0f);
    
    pos = mul(pos, World);
    pos = mul(pos, View);
    pos = mul(pos, Projection);
    
    output.Position = pos;
    output.Col = vOutputColor;
    
    return output;
}

// ----------------
//  PIXEKL SHADER
// ----------------
float4 PS(VS_OUT input) : SV_TARGET
{
    return input.Col;
}