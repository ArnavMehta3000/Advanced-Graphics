#include "Structures.hlsli"

// https://www.cs.utexas.edu/~fussell/courses/cs384g-spring2016/lectures/normal_mapping_tangent.pdf

Texture2D txDiffuse    : register(t0);
Texture2D txNormal     : register(t1);
Texture2D txHeight     : register(t2);
SamplerState samLinear : register(s0);
// ---------------------------------------------------------------------



// ------------
//  STRUCTURES
// ------------
struct VS_IN
{
    float3 Position  : POSITION;
    float3 Normal    : NORMAL;
    float2 UV        : TEXCOORD0;
    float3 Tangent   : TANGENT;
    float3 Binormal  : BINORMAL;
};

struct PS_IN
{
    float4 Position  : SV_POSITION;
    float4 PositionW : POSITION;
    float3 PositionT : POSITIONT;
    float3 Normal    : NORMAL;
    float2 UV        : TEXCOORD0;
    float3 NormalT   : NORMALT;
    float3 LightDirT : TLIGHTDIR;
    float3 EyeDirT   : TEYEDIR;
    float3 EyePosT   : EYEPOSITIONT;
};
// ---------------------------------------------------------------------


// ------------------
//  CONSTANT BUFFERS
// ------------------
cbuffer ConstantBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

cbuffer MaterialProperties : register(b1)
{
    _Material Material;
};

cbuffer LightProperties : register(b2)
{
	float4 EyePosition;               // 16 bytes
    float4 GlobalAmbient = (float4)0; // 16 bytes
    PointLight Light;                 // 48 bytes
}; 
// ---------------------------------------------------------------------


// -----------
//  FUNCTIONS
// -----------
float3 ToTangentSpace(float3 v, float3x3 InvTBN)
{
    return normalize(mul(v, InvTBN));
}

LightingResult DoPointLight(float3 lightDir, float3 viewDir, float3 vertexPos, float3 vertexNormal)
{
    // Lighting calculation reference taken from HLSL Development Cookbook by Doron Feinstein
    LightingResult result;
    
    // Diffuse
    //float3 lightDir          = normalize(Light.Position.xyz - vertexPos);  //Unused for normal mapping
    float diffuseIntensity   = max(dot(lightDir, vertexNormal), 0.0f);
    result.Diffuse           = Light.Diffuse * diffuseIntensity;
    
    // Specular
    //float3 viewDir    = normalize(EyePosition.xyz - vertexPos);           // Unused for normal mapping
    float3 reflectDir = reflect(-lightDir, vertexNormal);
    float specular    = pow(max(dot(viewDir, reflectDir), 0.0f), Material.SpecularPower);
    result.Specular   = Light.Specular * specular;
    
    // Attenuation
    float distance    = length(Light.Position.xyz - vertexPos);
    float3 la         = Light.Attenuation.xyz;
    float attenuation = 1.0f / (la.x + (la.y * distance) + (la.z * distance * distance));

    result.Diffuse  *= attenuation;
    result.Specular *= attenuation;
    
    return result;
}
// ---------------------------------------------------------------------


// ----------------
//  VERTEX SHADER
// ----------------
PS_IN VS(VS_IN input)
{
    PS_IN output      = (PS_IN) 0;
    
    float4 pos        = float4(input.Position, 1.0f);
    
    output.Position   = mul(pos, World);
    output.PositionW  = output.Position;
    output.Position   = mul(output.Position, View);
    output.Position   = mul(output.Position, Projection);

    output.Normal     = mul(float4(input.Normal, 0), World).xyz;
    output.UV        = input.UV;
    
    // Create the TBN matrix
    float3 T          = normalize(mul(float4(input.Tangent,  0.0f), World)).xyz;
    float3 N          = normalize(mul(float4(input.Normal, 0), World).xyz);
    float3 B          = normalize(mul(float4(input.Binormal, 0.0f), World)).xyz;
    //T                 = normalize(T - dot(T, N) * N);  //Gram-Schmidt process
    float3x3 TBN      = float3x3(T, B, N);
    
    float3x3 invTBN   = transpose(TBN);
    
    float3 lightDir = normalize(Light.Position.xyz - output.PositionW.xyz);  // To light
    float3 viewDir  = normalize(EyePosition.xyz - output.PositionW.xyz);     // To Eye
    
    // Get tangent space vectors
    output.LightDirT = ToTangentSpace(lightDir, invTBN);
    output.EyeDirT   = ToTangentSpace(viewDir, invTBN);
    output.PositionT = ToTangentSpace(output.PositionW.xyz, invTBN);
    output.NormalT   = ToTangentSpace(output.Normal, invTBN);
    output.EyePosT   = ToTangentSpace(EyePosition.xyz, invTBN);
    
    return output;
}
// ---------------------------------------------------------------------


// ---------------
//  PIXEL SHADER
// ---------------
float4 PS(PS_IN input) : SV_TARGET
{
    LightingResult pointLight;
    float4 finalColor = (float)0;
    float4 texColor   = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    if (Material.UseTexture)
        texColor = txDiffuse.Sample(samLinear, input.UV);

    // This uses world space normal mapping
    if (Material.UseNormals)
    {
        // Uncompress the normals from the normal map
        float4 texNormal     = txNormal.Sample(samLinear, input.UV);
        float4 bumpNormalT   = float4(normalize(2.0f * texNormal.xyz - 1.0f).xyz, 1.0f);  // These normals are in tangent space
        
        pointLight = DoPointLight(input.LightDirT, input.EyeDirT, input.PositionW.xyz, bumpNormalT.xyz);
    }
    else
    {
        float3 lightDir = normalize(Light.Position.xyz - input.PositionW.xyz); // To light
        float3 viewDir  = normalize(EyePosition.xyz - input.PositionW.xyz);     // To Eye
        
        pointLight = DoPointLight(lightDir, viewDir, input.PositionW.xyz, normalize(input.Normal));    
    }

    float4 ambient  = GlobalAmbient;
    float4 diffuse  = Material.Diffuse * pointLight.Diffuse;
    float4 specular = Material.Specular * pointLight.Specular;
    
    
    finalColor = texColor * (ambient + diffuse + specular);
    return finalColor;
}
// ---------------------------------------------------------------------
