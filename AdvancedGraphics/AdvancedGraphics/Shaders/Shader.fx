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
    float2 UV        : TEXCOORD0;
    float3 NormalT   : NORMALT;
    float3 NormalW   : NORMALW;
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

float2 SimpleParallaxMapping(float2 uv, float3 viewDir)
{
    viewDir = normalize(viewDir);
    
    float heightScale = 0.01f;
    float bias        = 0.0f;  // -0.01 or -0.02
    float height      = txHeight.Sample(samLinear, uv).r;
    float2 offset     = viewDir.xy / viewDir.z * (height * heightScale);
    offset += bias;
    
    float2 result = uv - offset;  
   
    return result;
}

float2 CalculatePOMUVOffset(float2 offset, float2 uv, int stepCount)
{
    float currentHeight = 0.0f;
    float stepSize = 1.0f / (float)stepCount;
    float prevHeight = 1.0f;
    float nextHeight = 0.0f;
    int stepIndex = 0;

    float2 texOffsetPerStep = stepSize * offset;
    float2 texCurrentOffset = uv;
    float currentBound = 1.0f;
    float parallaxAmt = 0.0f;

    float2 pt1 = 0;
    float2 pt2 = 0;

    float2 texOffset2 = 0;
    float2 dx = ddx(uv);
    float2 dy = ddy(uv);

    // Ray march
    while(stepIndex < stepCount)
    {
        texCurrentOffset -= texOffsetPerStep;
        currentHeight = txHeight.SampleGrad(samLinear, texCurrentOffset, dx, dy).r;
        
        currentBound -= stepSize;

        if (currentHeight > currentBound)
        {
            pt1 = float2(currentBound, currentHeight);
            pt2 = float2(currentBound + stepSize, prevHeight);

            texOffset2 = texCurrentOffset - texOffsetPerStep;

            stepIndex = stepCount + 1;
        }
        else
        {
            stepIndex++;
            prevHeight = currentHeight;
        }
    }

    // Linearly  Interpolate
    float delta1 = pt1.x - pt1.y;
    float delta2 = pt2.x - pt2.y;
    float difference = delta2 - delta1;

    if (difference == 0.0f)
        parallaxAmt = 0.0f;
    else
        parallaxAmt = (pt1.x * delta2 - pt2.x * delta1) / difference;

    float2 parallaxOffset = offset * (1.0f - parallaxAmt);
    return uv - parallaxOffset;
}

int GetPOMRayStepCount(float3 viewDir, float3 normal)
{
    const int minLayers = 8;
    const int maxLayers = 32;

    
    float t = dot(normalize(viewDir), normal);
    int numLayers = (int)lerp(maxLayers, minLayers, t);
    
    return numLayers;
}

float CalculatePOMSelfShadow(float2 offset, float2 uv, int stepCount)
{
    float2 dx = ddx(uv);
    float2 dy = ddy(uv);

    float currentHeight = 0.0f;
    float stepSize = 1.0f / (float) stepCount;
    float stepIndex = 0.0f;

    float2 texOffsetPerStep = stepSize * offset;
    float2 texCurrentOffset = uv;
    float currentBound = txHeight.SampleGrad(samLinear, texCurrentOffset, dx, dy).r;
    float softShadow = 0.0f;

    while (stepIndex < stepCount)
    {
        texCurrentOffset += texOffsetPerStep * offset;
        currentHeight = txHeight.SampleGrad(samLinear, texCurrentOffset, dx, dy).r;

        currentBound += stepSize;

        if (currentHeight > currentBound)
        {
            float newSoftShadow = (currentHeight - currentBound) * (1.0f - (float)stepIndex / (float(stepCount)));
            softShadow = max(softShadow, newSoftShadow);
        }
        else
            stepIndex++;

        float softShadowFactor = 0.01f;
        float shadow = (stepIndex >= stepCount) ? ((1.0f - clamp(softShadowFactor * softShadow, 0.0f, 1.0f))) : 1.0f;
        return shadow;
    }
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
        // float2 texCoords = SimpleParallaxMapping(input.UV, input.EyeDirT);
        float2 texCoords = CalculatePOMUVOffset(float2(0.01,0.01), input.UV, GetPOMRayStepCount(input.EyeDirT, input.NormalT));
    
        //if (texCoords.x > 1.0f || texCoords.y > 1.0f || texCoords.x > 0.0f || texCoords.y > 0.0f)
          //  discard;
    }
    
    if (Material.UseTexture)
        texColor = txDiffuse.Sample(samLinear, texCoords);

    // This uses world space normal mapping
    if (Material.UseNormals)
    {
    // Uncompress the normals from the normal map (in tangent space)
        float4 texNormal = txNormal.Sample(samLinear, texCoords);
        float4 bumpNormalT = float4(normalize(2.0f * texNormal.xyz - 1.0f).xyz, 1.0f);
    
        pointLight = DoPointLight(input.LightDirT, input.EyeDirT, input.PositionW.xyz, bumpNormalT.xyz);
    }
    else
    {
        pointLight = DoPointLight(input.LightDirT, input.EyeDirT, input.PositionW.xyz, input.NormalT.xyz);
    }

    float4 ambient = GlobalAmbient;
    float4 diffuse = Material.Diffuse * pointLight.Diffuse;
    float4 specular = Material.Specular * pointLight.Specular;
    
    
    finalColor = texColor * (ambient + diffuse + specular);
    return finalColor;
    }
// ---------------------------------------------------------------------
