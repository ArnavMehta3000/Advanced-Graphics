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
    float4 Position;     // 16 bytes
    float4 Color;        // 16 bytes
    float  Range;        // 4 bytes
    float  Attenuation;  // 4 bytes
    float  Power;      // 8 bytes
    float  Padding;      // 8 bytes
};

struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};