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
    float4 Position        : SV_POSITION;
    float4 PositionW       : POSITION;
    float3 PositionT       : POSITIONT;
    float2 UV              : TEXCOORD0;
    float3 NormalT         : NORMALT;
    float3 NormalW         : NORMALW;
    float3 LightDirT       : TLIGHTDIR;
    float3 EyeDirT         : TEYEDIR;
    float3 EyePosT         : EYEPOSITIONT;
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

bool IsUVInBounds(float2 uv)
{
    if (uv.x > 1.0f || uv.y > 1.0f || uv.x < 0.0f || uv.y < 0.0f)
        return false;
    else
        return true;
}

LightingResult CalculatePointLight(float3 lightDir, float3 viewDir, float3 vertexPos, float3 vertexNormal)
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
// Ref: https://shaderbits.com/blog/curved-surface-parallax-occlusion-mapping#:~:text=Parallax%20Mapping%20refers%20to%20the,height%20field%20checking%20for%20intersections.
float2 SimpleParallaxMapping(float2 uv, float3 viewDir)
{
    viewDir = normalize(viewDir);
    
    const float heightScale = 0.05f;
    const float bias        = -0.01f;  // -0.01 or -0.02

    float height  = txHeight.Sample(samLinear, uv).r;
    float2 offset = viewDir.xy / viewDir.z * (height * heightScale);
    
    offset += bias;
    
    float2 result = uv - offset;  
    return result;
}

float2 CalculatePOM(float2 uv, float3 viewDir, float3 normal)
{
    viewDir = normalize(viewDir);
    // Compute all the derivatives
    float2 dx = ddx(uv);
    float2 dy = ddy(uv);

    float minLayers   = Light.Parallax.x;
    float maxLayers   = Light.Parallax.y;
    float heightScale = Light.Parallax.z;
    float bias        = Light.Parallax.w;  // Maybe convert bias to float 2 for X/Y Axis?

    float numLayers         = lerp(maxLayers, minLayers, abs(dot(viewDir, normal)));
    float layerDepth        = 1.0 / numLayers;
    float currentLayerDepth = 0.0;

    float2 offset         = viewDir.xy / viewDir.z * heightScale;
    offset               += bias;
    float2 deltaTexCoords = offset / numLayers;

    float2 currentTexCoords    = uv;
    float currentDepthMapValue = txHeight.SampleGrad(samLinear, currentTexCoords, dx, dy).r;

    while(currentLayerDepth < currentDepthMapValue)
    {
        currentTexCoords -= deltaTexCoords;
        currentDepthMapValue = txHeight.SampleGrad(samLinear, currentTexCoords, dx, dy).r;
        currentLayerDepth += layerDepth;
    }

    float2 prevTexCoords = currentTexCoords + deltaTexCoords;

    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = txHeight.SampleGrad(samLinear, currentTexCoords, dx, dy).r - currentLayerDepth + layerDepth;

    float weight          = afterDepth / (afterDepth - beforeDepth);
    float2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
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
    float3 normal   = normalize(mul(float4(input.Normal, 0), World).xyz);
    output.NormalW  = normal;
    
    
    // Get tangent space vectors
    output.LightDirT = ToTangentSpace(lightDir, invTBN);
    output.EyeDirT   = ToTangentSpace(viewDir, invTBN);    
    output.PositionT = ToTangentSpace(output.PositionW.xyz, invTBN);
    output.NormalT   = ToTangentSpace(normal, invTBN);
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
    
    float2 texCoords = input.UV;
    
    if (Material.UseHeight)
    {
        // texCoords = SimpleParallaxMapping(input.UV, input.EyeDirT);
        texCoords = CalculatePOM(input.UV, input.EyeDirT, input.NormalT);

        // Discard pixel if uv is not in bounds
        if (!IsUVInBounds(texCoords))
            discard;
    }
    
    if (Material.UseTexture)
        texColor = txDiffuse.Sample(samLinear, texCoords);

    // This uses world space normal mapping
    if (Material.UseNormals)
    {
    // Uncompress the normals from the normal map (in tangent space)
        float4 texNormal = txNormal.Sample(samLinear, texCoords);
        float4 bumpNormalT = float4(normalize(2.0f * texNormal.xyz - 1.0f).xyz, 1.0f);
    
        pointLight = CalculatePointLight(input.LightDirT, input.EyeDirT, input.PositionW.xyz, bumpNormalT.xyz);
    }
    else
    {
        pointLight = CalculatePointLight(input.LightDirT, input.EyeDirT, input.PositionW.xyz, input.NormalT.xyz);
    }

    float4 ambient = GlobalAmbient;
    float4 diffuse = Material.Diffuse * pointLight.Diffuse;
    float4 specular = Material.Specular * pointLight.Specular;
    
    
    finalColor = texColor * (ambient + diffuse + specular);
    return finalColor;
    }
// ---------------------------------------------------------------------
