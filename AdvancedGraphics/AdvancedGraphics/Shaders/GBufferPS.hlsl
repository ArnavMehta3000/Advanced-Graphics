
struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 PositionW : POSITION;
    float3 PositionT : POSITIONT;
    float2 UV : TEXCOORD0;
    float3 NormalT : NORMALT;
    float3 NormalW : NORMALW;
    float3 LightDirT : TLIGHTDIR;
    float3 EyeDirT : TEYEDIR;
    float3 EyePosT : EYEPOSITIONT;
};

struct PS_OUTPUT
{
    float4 Diffuse  : SV_Target0;
    float4 Normal   : SV_Target1;
    float4 Position : SV_Target2;
};


PS_OUTPUT PS(VS_OUTPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0;
    
    output.Diffuse = float4(1, 1, 1, 1);
    output.Normal  = float4(input.NormalW.xyz, 1.0f);
    output.Position = input.PositionW;
    
    return output;
}