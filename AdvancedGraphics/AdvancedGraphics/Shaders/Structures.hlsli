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
    float4 Position;    // 16 bytes
    float4 Diffuse;     // 16 bytes
    float4 Specular;    // 16 bytes
    float4 Attenuation; // 16 bytes  (w component not used)
};

struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};