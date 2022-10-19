struct _Material
{
    float4 Diffuse;       // 16 bytes
    float4 Specular;      // 16 bytes
    float  SpecularPower; // 4 bytes
    bool   UseTexture;    // 4 bytes
    bool   UseNormals;    // 4 bytes
    float  Padding;       // 4 bytes
};

struct PointLight
{
    float3 Position;     // 12 bytes
    float3 Color;        // 12 bytes
    float  Attenuation;  // 4 bytes
    float  Padding;      // 4 bytes
};