struct _Material
{
    float4 Diffuse;       // 16 bytes
    float4 Specular;      // 16 bytes
    float  SpecularPower; //  4 bytes
    bool   UseTexture;    //  4 bytes
    bool   UseNormals;    //  4 bytes
    bool   UseHeight;     //  4 bytes
};

struct PointLight
{
    float4 Position;    // 16 bytes
    float4 Diffuse;     // 16 bytes
    float4 Specular;    // 16 bytes
    float4 Attenuation; // 16 bytes  (w component not used)
    float4 Parallax;    // (x: minLayers | y : maxLayers | z : heightScale | w : shadowFactor)
	float4 Bias;        // (x: biasX | y : biasY | z : not used| w : not used)
};

struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};

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
    float3x3 TBN : TBN;
};
