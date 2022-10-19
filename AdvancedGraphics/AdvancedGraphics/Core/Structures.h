#pragma once
struct SimpleVertex
{
	sm::Vector3 Pos;
	sm::Vector3 Normal;
	sm::Vector2 TexCoord;
	sm::Vector3 Tangent;
	sm::Vector3 Binormal;
};

// Buffer of data used to set the WVP matrices in the vertex shader
struct VSConstantBuffer
{
	sm::Matrix World;
	sm::Matrix View;
	sm::Matrix Projection;
};

struct _Material
{
	_Material()
		:
		Diffuse(1.0f, 1.0f, 1.0f, 1.0f),
		Specular(1.0f, 1.0f, 1.0f, 1.0f),
		SpecularPower(128.0f),
		UseTexture(false)
	{}


	sm::Vector4 Diffuse;
	sm::Vector4 Specular;
	float       SpecularPower;
	int         UseTexture;
	int         UseNormals;
	float       Padding;
};

struct MaterialProperties
{
	_Material Material;
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
		Color(1.0f, 1.0f, 1.0f, 1.0f)
	{}
	sm::Vector4 Position;
	sm::Vector4	Color;
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