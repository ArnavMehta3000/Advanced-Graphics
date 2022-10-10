#pragma once
struct SimpleVertex
{
	sm::Vector3 Pos;
	sm::Vector3 Normal;
	sm::Vector2 TexCoord;
};

// Buffer of data used to set the WVP matrices in the vertex shader
struct VSConstantBuffer
{
	sm::Matrix World;
	sm::Matrix View;
	sm::Matrix Projection;
	sm::Color OutputColor;
};

struct _Material
{
	_Material()
		: Emissive(0.0f, 0.0f, 0.0f, 1.0f)
		, Ambient(0.1f, 0.1f, 0.1f, 1.0f)
		, Diffuse(1.0f, 1.0f, 1.0f, 1.0f)
		, Specular(1.0f, 1.0f, 1.0f, 1.0f)
		, SpecularPower(128.0f)
		, UseTexture(false)
	{}

	sm::Vector4 Emissive;
	sm::Vector4 Ambient;
	sm::Vector4 Diffuse;
	sm::Vector4 Specular;
	float       SpecularPower;
	int         UseTexture;
	float       Padding[2];
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


#define POINT_LIGHTS 1
#define DIRECTIONAL_LIGHTS 0
#define MAX_LIGHTS POINT_LIGHTS + DIRECTIONAL_LIGHTS

struct Light
{
	Light()
		: Position(0.0f, 0.0f, 0.0f, 1.0f)
		, Direction(0.0f, 0.0f, 1.0f, 0.0f)
		, Color(1.0f, 1.0f, 1.0f, 1.0f)
		, SpotAngle(DirectX::XM_PIDIV2)
		, ConstantAttenuation(1.0f)
		, LinearAttenuation(0.0f)
		, QuadraticAttenuation(0.0f)
		, LightType((int)LightType::DirectionalLight)
		, Enabled(0)
	{}

	sm::Vector4 Position;
	sm::Vector4 Direction;
	sm::Vector4 Color;
	float       SpotAngle;
	float       ConstantAttenuation;
	float       LinearAttenuation;
	float       QuadraticAttenuation;
	int         LightType;
	int         Enabled;
	float       Range;
	float       Padding;
};

struct LightProperties
{
	LightProperties()
		: EyePosition(0.0f, 0.0f, 0.0f, 1.0f)
		, GlobalAmbient(0.1f, 0.1f, 0.1f, 1.0f)
	{}

	sm::Vector4 EyePosition;
	sm::Vector4 GlobalAmbient;
	Light       Lights[MAX_LIGHTS];
};

