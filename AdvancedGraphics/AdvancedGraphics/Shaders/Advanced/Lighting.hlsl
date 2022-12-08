#define MAX_LIGHTS 1

cbuffer WVPBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;
}

struct Light
{
    float4 Position;
    float4 Diffuse;
    float4 Specular;
};

cbuffer LightCameraBuffer : register(b1)
{
    matrix InvView;
    matrix InvProjection;
    float4 EyePosition;
    Light Lights[MAX_LIGHTS];
}

Texture2D<float4> GDiffuse  : register(t0);
Texture2D<float4> GNormal   : register(t1);
Texture2D<float4> GPosition : register(t2);
SamplerState samLinear      : register(s0);


struct VSInput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : UV;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 TexCoord : UV;
};

struct SurfaceData
{
    //float Depth;
    float4 Position;
    float4 Diffuse;
    float4 Normal;
};



VSOutput VS(VSInput input)
{
    VSOutput output;
    output.Position = input.Position;
    output.TexCoord = input.TexCoord;
    return output;
}

float4 GetPositionFromDepth(int3 uv, VSOutput input, float depth)
{
    if (depth == 0.0f)
        discard;
    
    float4 viewDir = float4(input.Position.xyz / input.Position.w, 1.0f);
    
    
    return depth * viewDir + EyePosition;
}

SurfaceData UnpackGBuffer(VSOutput input)
{
    int3 pos = int3((int2) input.Position.xy, 0);
    
    SurfaceData output;
   // output.Depth    = GDepth.Load(pos);
    output.Position = GPosition.Load(pos); //GetPositionFromDepth(pos, input, output.Depth);
    output.Diffuse  = GDiffuse.Load(pos);
    output.Normal   = GNormal.Load(pos);
    
    return output;
}


float4 PS(VSOutput input) : SV_Target0
{
    return UnpackGBuffer(input).Position * Lights[0].Diffuse;
}

// Assuming "matView" is your current view matrix.D3DXMATRIX matInverseView;
//D3DXMatrixInverse( &matInverseView, NULL, &matView );
//D3DXVECTOR3 vDir = D3DXVECTOR3( matInverseView[2][0], matInverseView[2][1], matInverseView[2][2] );