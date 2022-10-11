// ------------
//  STRUCTURES
// ------------
#define MAX_LIGHTS 1
// Light types.
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT 1
#define SPOT_LIGHT 2

Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

struct _Material
{
    float4 Emissive;      // 16 bytes
    float4 Ambient;       // 16 bytes
    float4 Diffuse;       // 16 bytes
    float4 Specular;      // 16 bytes
    float  SpecularPower; // 4 bytes
    bool   UseTexture;    // 4 bytes
    float2 Padding;       // 16 bytes
};

struct Light
{
    float4 Position;             // 16 bytes
    float4 Direction;            // 16 bytes
    float4 Color;                // 16 bytes
    float  SpotAngle;            // 4 bytes
    float  ConstantAttenuation;  // 4 bytes
    float  LinearAttenuation;    // 4 bytes
    float  QuadraticAttenuation; // 4 bytes
    int    LightType;            // 4 bytes
    bool   Enabled;              // 4 bytes
    float  Range;                // 4 bytes
    float  Padding;              // 4 bytes
};

struct VS_IN
{
    float3 Position : POSITION;
    float3 Norm     : NORMAL;
    float2 Tex      : TEXCOORD0;
};

struct PS_IN
{
    float4 Position : SV_POSITION;
    float4 worldPos : POSITION;
    float3 Norm     : NORMAL;
    float2 Tex      : TEXCOORD0;
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

cbuffer MaterialProperties : register(b1)
{
    _Material Material;
};

cbuffer LightProperties : register(b2)
{
	float4 EyePosition;        // 16 bytes
	float4 GlobalAmbient;      // 16 bytes
	Light Lights[MAX_LIGHTS];  // 80 * 8 = 640 bytes
}; 



// -----------
//  FUNCTIONS
// -----------
float4 DoDiffuse(Light light, float3 L, float3 N)
{
    float NdotL = max(0, dot(N, L));
    return light.Color * NdotL;
}

float4 DoSpecular(Light lightObject, float3 vertexToEye, float3 lightDirectionToVertex, float3 Normal)
{
    float4 lightDir = float4(normalize(-lightDirectionToVertex), 1);
    vertexToEye = normalize(vertexToEye);

    float lightIntensity = saturate(dot(Normal, lightDir.xyz));
    float4 specular = float4(0, 0, 0, 0);
    if (lightIntensity > 0.0f)
    {
        float3 reflection = normalize(2 * lightIntensity * Normal - lightDir.xyz);
        specular = pow(saturate(dot(reflection, vertexToEye)), Material.SpecularPower); // 32 = specular power
    }

    return specular;
}

float DoAttenuation(Light light, float d)
{
    return 1.0f / (light.ConstantAttenuation + light.LinearAttenuation * d + light.QuadraticAttenuation * d * d);
}

struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};

LightingResult DoPointLight(Light light, float3 vertexToEye, float4 vertexPos, float3 N)
{
    LightingResult result;

    float3 LightDirectionToVertex = (vertexPos - light.Position).xyz;
    float distance                = length(LightDirectionToVertex);
    LightDirectionToVertex        = LightDirectionToVertex / distance;

    float3 vertexToLight = (light.Position - vertexPos).xyz;
    distance             = length(vertexToLight);
    vertexToLight        = vertexToLight / distance;

    float attenuation = DoAttenuation(light, light.Range);


    result.Diffuse  = DoDiffuse(light, vertexToLight, N) * attenuation;
    result.Specular = DoSpecular(light, vertexToEye, LightDirectionToVertex, N) * attenuation;

    return result;
}

LightingResult ComputeLighting(float4 vertexPos, float3 N)
{
    float3 vertexToEye = normalize(EyePosition - vertexPos).xyz;

    LightingResult totalResult = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

	[unroll]
    for (int i = 0; i < MAX_LIGHTS; ++i)
    {
        LightingResult result = { { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };

        if (!Lights[i].Enabled) 
            continue;
		
        result = DoPointLight(Lights[i], vertexToEye, vertexPos, N);
		
        totalResult.Diffuse  += result.Diffuse;
        totalResult.Specular += result.Specular;
    }

    totalResult.Diffuse  = saturate(totalResult.Diffuse);
    totalResult.Specular = saturate(totalResult.Specular);

    return totalResult;
}

// ----------------
//  VERTEX SHADER
// ----------------
PS_IN VS(VS_IN input)
{
    PS_IN output      = (PS_IN) 0;
    
    float4 pos = float4(input.Position, 1.0f);
    
    output.Position   = mul(pos, World);
    output.worldPos   = output.Position;
    output.Position   = mul(output.Position, View);
    output.Position   = mul(output.Position, Projection);

	// multiply the normal by the world transform (to go from model space to world space)
    output.Norm = mul(float4(input.Norm, 0), World).xyz;

    output.Tex = input.Tex;

    return output;
}

// ---------------
//  PIXEL SHADER
// ---------------

float4 PS(PS_IN IN) : SV_TARGET
{
    LightingResult lit = ComputeLighting(IN.worldPos, normalize(IN.Norm));

    float4 texColor = { 1, 1, 1, 1 };

    float4 emissive = Material.Emissive;
    float4 ambient  = Material.Ambient * GlobalAmbient;
    float4 diffuse  = Material.Diffuse * lit.Diffuse;
    float4 specular = Material.Specular * lit.Specular;

    if (Material.UseTexture)
        texColor = txDiffuse.Sample(samLinear, IN.Tex);

    float4 finalColor = (emissive + ambient + diffuse + specular) * texColor;

    return finalColor;
}

float4 PSSolid(PS_IN input) : SV_TARGET
{
    return vOutputColor;
}