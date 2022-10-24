#include "Structures.hlsli"

// https://www.cs.utexas.edu/~fussell/courses/cs384g-spring2016/lectures/normal_mapping_tangent.pdf

Texture2D txDiffuse    : register(t0);
Texture2D txNormal     : register(t1);
SamplerState samLinear : register(s0);
// ---------------------------------------------------------------------



// ------------
//  STRUCTURES
// ------------
struct VS_IN
{
    float3 Position  : POSITION;
    float3 Norm      : NORMAL;
    float2 Tex       : TEXCOORD0;
    float3 Tangent   : TANGENT;
    float3 Binormal  : BINORMAL;
};

struct PS_IN
{
    float4   Position  : SV_POSITION;
    float4   PositionW : POSITION;
    float3   Norm      : NORMAL;
    float2   Tex       : TEXCOORD0;
    float3   LightDirT : TLIGHTDIR;
    float3   EyeDirT   : TEYEDIR;
    float3x3 TBN       : TBN;
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

LightingResult CalculatePointLight(float3 vertexToEye, float3 vertexPos, float3 normal)
{
    LightingResult result = (LightingResult) 0;
	
    float3 lightDirectionToVertex = vertexPos - Light.Position.xyz;
    float distance = length(lightDirectionToVertex);
    lightDirectionToVertex /= distance; // normalize
	
    float3 vertexToLight = Light.Position.xyz - vertexPos;
    distance = length(vertexToLight);
    vertexToLight /= distance;
	
    float3 lightAtten = Light.Attenuation.xyz;
    float attenuation = 1.0f / (lightAtten.x + (lightAtten.y * distance) + (lightAtten.z * distance * distance));
	
	// Diffuse
    float NDotL = max(dot(normal, vertexToLight), 0.0f);
    float4 diffuse = float4(Light.Diffuse.xyz * NDotL, 1.0f);
	
	// Specular
    float4 lightDir = float4(normalize(-lightDirectionToVertex), 1.0f);
    vertexToEye = normalize(vertexToEye);
	
    float lightIntensity = saturate(dot(normal, lightDir.xyz));
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (lightIntensity > 0.0f)
    {
        float3 reflection = normalize(2 * lightIntensity * normal - lightDir.xyz);
        specular = pow(saturate(dot(reflection, vertexToEye)), Material.SpecularPower);
        specular = Light.Specular * specular;
    }
	
    result.Diffuse = diffuse * attenuation;
    result.Specular = specular * attenuation;
	
    return result;
}

LightingResult CalculateLighting(float3 vertexPos, float3 normal, float3 viewDir)
{
    LightingResult result = (LightingResult) 0;
	
	// This is in tangent space
    //float3 vertexToEye = normalize(vertextoeyets - vertexPos).xyz;
    float3 vertexToEye = normalize(EyePosition.xyz - vertexPos);
	
    result = CalculatePointLight(viewDir, vertexPos, normal);
	
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

    output.Norm       = mul(float4(input.Norm, 0), World).xyz;
    output.Tex        = input.Tex;
    
    // Create the TBN matrix
    float3 T          = normalize(mul(float4(input.Tangent,  0.0f), World)).xyz;
    float3 N          = output.Norm;
    float3 B          = cross(T, N); //normalize(mul(float4(input.Binormal, 0.0f), World)).xyz;
    T                 = normalize(T - dot(T, N) * N);  //Gram-Schmidt process
    float3x3 TBN      = float3x3(T, B, N);
    
    float3x3 invTBN   = transpose(TBN);
    output.TBN        = TBN;
    
    float3 lightDir = normalize(Light.Position.xyz - output.PositionW.xyz);  // To light
    float3 viewDir  = normalize(EyePosition.xyz - output.PositionW.xyz);     // To Eye
    
    // Convert eye and light vectors to tangent space
    output.LightDirT = ToTangentSpace(lightDir, invTBN);
    output.EyeDirT   = ToTangentSpace(viewDir, invTBN);
    
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
        texColor = txDiffuse.Sample(samLinear, input.Tex);

    // This uses world space normal mapping
    if (Material.UseNormals)
    {
        // Uncompress the normals from the normal map
        float4 texNormal     = txNormal.Sample(samLinear, input.Tex);
        float4 bumpNormalT   = float4(normalize(2.0f * texNormal.xyz - 1.0f).xyz, 1.0f);  // These normals are in tangent space
        float3 bumpNormalW   = mul(bumpNormalT.xyz, input.TBN);
        
        // For testing purposes
        float3 lightDir = normalize(Light.Position.xyz - input.PositionW.xyz); // To light
        float3 viewDir  = normalize(EyePosition.xyz - input.PositionW.xyz);    // To Eye
        
        pointLight = DoPointLight(lightDir, viewDir, input.PositionW.xyz, bumpNormalW.xyz);
    }
    else
    {
        float3 lightDir = normalize(Light.Position.xyz - input.PositionW.xyz); // To light
        float3 viewDir  = normalize(EyePosition.xyz - input.PositionW.xyz);     // To Eye
        
        pointLight = DoPointLight(lightDir, viewDir, input.PositionW.xyz, normalize(input.Norm));    
    }
    
    // This is the new lighting copied from the given framework
    pointLight = CalculateLighting(input.PositionW.xyz, input.Norm, normalize(EyePosition.xyz - input.PositionW.xyz));
    

    float4 ambient  = GlobalAmbient;
    float4 diffuse  = Material.Diffuse * pointLight.Diffuse;
    float4 specular = Material.Specular * pointLight.Specular;
    
    
    finalColor = texColor * (ambient + diffuse + specular);
    return finalColor;
}
// ---------------------------------------------------------------------
