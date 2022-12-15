#pragma once
#define MAX_LIGHTS 1

struct SimpleVertex
{
	sm::Vector3 Pos;
	sm::Vector3 Normal;
	sm::Vector2 TexCoord;
	sm::Vector3 Tangent;
	sm::Vector3 Binormal;
};

struct FSQuadVertex
{
	XMFLOAT4 Pos;
	XMFLOAT2 TexCoord;
};

// Buffer of data used to set the WVP matrices in the vertex shader
struct WVPBuffer
{
	sm::Matrix World;
	sm::Matrix View;
	sm::Matrix Projection;
};

struct _Material
{
	sm::Vector4 Diffuse;       
	sm::Vector4 Specular;      
	float       SpecularPower; 
	int         UseTexture;    
	int         UseNormals;    
	int         UseHeight;

	_Material()
		:
		Diffuse(1.0f),
		Specular(Colors::LightGreen),
		SpecularPower(10.0f),
		UseTexture(true),
		UseNormals(true),
		UseHeight(true)
	{}
};


struct MaterialProperties
{
	_Material Material;
};

struct PostProcessing
{
	sm::Matrix CurrentViewProjection;
	sm::Matrix PrevViewProjection;
	int EnableVignette;
	int EnableGrayscale;
	sm::Vector2 VignetteRadiusSoftness;
};

struct Light
{
	sm::Vector4 Position;
	sm::Vector3 Diffuse;
	float Intensity;

	sm::Vector3 Specular;
	float Radius;

	sm::Vector4 Parallax;
	sm::Vector4 Bias;

	float SpecularPower;
	sm::Vector3 _padding0;
};

struct LightBuffer
{
	sm::Vector4 EyePosition;
	sm::Vector4 GlobalAmbient;
	Light PointLight;
};

struct SurfaceProperties
{
	SurfaceProperties()
		:
		SpecularColor(1.0f),
		SpecularPower(32.0f)
	{}

	sm::Vector3 SpecularColor;
	float SpecularPower;
};

enum class LightType
{
	DirectionalLight = 0,
	PointLight = 1,
	SpotLight = 2
};


struct PointLight
{
	PointLight()
		:
		Position(0.0f, 0.0f, -2.0f, 1.0f),
		Diffuse(1.0f, 1.0f, 1.0f, 1.0f),
		Specular(1.0f, 1.0f, 1.0f, 1.0f),
		Attenuation(1.0f, 1.0f, 1.0f, 1.0f)
	{}

	sm::Vector4 Position;
	sm::Vector4	Diffuse;
	sm::Vector4	Specular;
	sm::Vector4	Attenuation;  // w component not used
	sm::Vector4 Parallax;     // (x: minLayers | y : maxLayers | z : heightScale | w : shadowFactor)
	sm::Vector4 Bias;         // (x: biasX | y : biasY | z : not used| w : not used)
};

struct LightProperties
{
	LightProperties()
		: EyePosition(0.0f, 0.0f, 0.0f, 1.0f)
		, GlobalAmbient(0.01f, 0.01f, 0.01f, 1.0f)
	{}

	sm::Vector4 EyePosition;
	sm::Vector4 GlobalAmbient;
	PointLight  PointLight;
};
