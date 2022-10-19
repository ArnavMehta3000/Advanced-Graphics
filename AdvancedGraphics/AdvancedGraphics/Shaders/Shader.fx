#include "Structures.hlsli"



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
    float3   ToEyeT    : EYETANGENT;
    float3   ToLightT  : LIGHTTANGENT;
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
	float4     EyePosition;    // 16 bytes
	float4     GlobalAmbient;  // 16 bytes
    PointLight Light;          // 48 bytes
}; 
float3 ToTangentSpace(float3 v, float3x3 InvTBN)
{
    return normalize(mul(v, InvTBN));
}
// ---------------------------------------------------------------------


// -----------
//  FUNCTIONS
// -----------
float4 CalculatePointLights(float3 vertexPos, float3 vertexNormal)
{
    // Lighting calculation reference taken from HLSL Development Cookbook by Doron Feinstein
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);


    // Calculate to light
    float3 toLight = Light.Position.xyz - vertexPos;

    // Calculate to eye
    float3 toEye = EyePosition.xyz - vertexPos;
    
    // Get distance to light
    float distToLight = length(toLight);
    
    // ----- Phong Diffuse -----
    // Normalize to light
    toLight /= distToLight;
    
    // Get light intensity for pixel
    float NDotL = dot(toLight, vertexNormal);
    NDotL = saturate(NDotL);
    
    // Set final color for diffuse lighting
    float4 diffuse = float4(Light.Color.rgb * NDotL, 1.0f);
    finalColor += diffuse;
    
    // ----- Blinn specular -----
    // Normalize to eye
    toEye = normalize(toEye);
    
    // Get halfway point 
    float3 halfway = normalize(toEye + toLight);

    // Get specular intensity
    float NDotH = dot(halfway, vertexNormal);
    
    // Add specular lighting to final color
    //float4 specular = float4(Light.Color * pow(Material.Specular, Material.SpecularPower).xyz * finalColor.rgb, 1.0f);
    //finalColor += specular;
    
    // Attenuation
    //float attenuation = Light.Attenuation.x + (Light.Attenuation.y * distToLight) + (Light.Attenuation.z * distToLight * distToLight);
    //finalColor *= attenuation;
    
    
    //finalColor *= attenuation;
    
    
    return finalColor;
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
    float3 B          = normalize(mul(float4(input.Binormal, 0.0f), World)).xyz;
    float3 N          = output.Norm;
    float3x3 TBN      = float3x3(T, B, N);
    output.TBN        = TBN;
    
    float3x3 invTBN   = transpose(TBN);
    float3 toEye      = normalize(EyePosition.xyz - input.Position);
    float3 toLight    = normalize(Light.Position.xyz - input.Position);
    
    // Convert eye and light vectors to tangent space
    output.ToEyeT     = ToTangentSpace(toEye, invTBN);
    output.ToLightT   = ToTangentSpace(toLight, invTBN);
    
    return output;
}
// ---------------------------------------------------------------------


// ---------------
//  PIXEL SHADER
// ---------------
float4 PS(PS_IN input) : SV_TARGET
{
    float4 pointLight;
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
    float4 texColor   = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    if (Material.UseTexture)
        texColor = txDiffuse.Sample(samLinear, input.Tex);
    
    //if (Material.UseNormals)
    //{
    //    // Uncompress normals
    //    float4 texNormal     = txNormal.Sample(samLinear, IN.Tex);
    //    float3 bumpedNormalW = normalize(2.0f * texNormal.xyz - 1.0f);
    //    //NormalSampleToWorldSpace(texNormal.xyz, normalize(IN.Norm), normalize(IN.Tangent), normalize(IN.Binormal));
        
    //    lit = ComputeLighting(IN.PositionW, normalize(bumpedNormalW), IN.ToEyeT, IN.ToLightT);
        
    //    float3 toEye = normalize(EyePosition.xyz - IN.PositionW.xyz);
    //    float3 toLight = normalize(PointLight.Position.xyz - IN.PositionW.xyz);
        
    //    lit = DoMyLight(PointLight, IN.PositionW.xyz, toEye, toLight, normalize(IN.Norm));
    //}
    //else
    //    lit = ComputeLighting(IN.PositionW, normalize(IN.Norm), IN.ToEyeT, IN.ToLightT);

    pointLight = CalculatePointLights(input.PositionW.xyz, input.Norm);
    //(emissive + ambient + diffuse + specular) * texColor;

    finalColor = texColor * (pointLight + GlobalAmbient);
    return finalColor;
}
// ---------------------------------------------------------------------
