cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Normal   : NORMAL;
    float2 UV       : TEXCOORD0;
    float3 Tangent  : TANGENT;
    float3 Binormal : BINORMAL;
};

struct VS_OUTPUT
{
    float4 Position  : SV_POSITION;
    float4 PositionW : POSITION;
    float3 PositionT : POSITIONT;
    float2 UV        : TEXCOORD0;
    float3 NormalT   : NORMALT;
    float3 NormalW   : NORMALW;
    float3 LightDirT : TLIGHTDIR;
    float3 EyeDirT   : TEYEDIR;
    float3 EyePosT   : EYEPOSITIONT;
};


VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    
    float4 pos = float4(input.Position, 1.0f);
    
    output.Position  = mul(pos, World);
    output.PositionW = output.Position;
    output.Position  = mul(output.Position, View);
    output.Position  = mul(output.Position, Projection);

    output.UV        = input.UV;
    
    // Create the TBN matrix
    float3 T         = normalize(mul(float4(input.Tangent, 0.0f), World)).xyz;
    float3 N         = normalize(mul(float4(input.Normal, 0), World).xyz);
    float3 B         = normalize(mul(float4(input.Binormal, 0.0f), World)).xyz;
    float3x3 TBN     = float3x3(T, B, N);
    
    float3x3 invTBN  = transpose(TBN);
    
    //float3 lightDir  = normalize(Light.Position.xyz - output.PositionW.xyz); // To light
    //float3 viewDir   = normalize(EyePosition.xyz - output.PositionW.xyz); // To Eye
    float3 normal = normalize(mul(float4(input.Normal, 0), World).xyz);
    output.NormalW   = normal;
    
    
    // Get tangent space vectors
    //output.LightDirT = ToTangentSpace(lightDir, invTBN);
    //output.EyeDirT   = ToTangentSpace(viewDir, invTBN);
    //output.PositionT = ToTangentSpace(output.PositionW.xyz, invTBN);
    //output.NormalT   = ToTangentSpace(normal, invTBN);
    //output.EyePosT   = ToTangentSpace(EyePosition.xyz, invTBN);

    return output;
}