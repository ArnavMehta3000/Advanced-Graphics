struct _Material
{
    float4 Diffuse;       // 16 bytes
    float4 Specular;      // 16 bytes
    float  SpecularPower; // 4 bytes
    bool   UseTexture;    // 4 bytes
    bool   UseNormals;    // 4 bytes
    bool   UseHeight;     // 4 bytes
};

struct PointLight
{
    float4 Position;    // 16 bytes
    float4 Diffuse;     // 16 bytes
    float4 Specular;    // 16 bytes
    float4 Attenuation; // 16 bytes  (w component not used)
    float4 Parallax;    // 16 bytes (x: minLayers | y: maxLayers | z: heightScale | w: bias)
};

struct LightingResult
{
    float4 Diffuse;
    float4 Specular;
};
