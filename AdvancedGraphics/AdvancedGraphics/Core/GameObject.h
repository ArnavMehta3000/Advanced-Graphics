#pragma once
#include "Graphics/Mesh.h"
#include "Graphics/Primitives.h"

struct GODesc
{
	const CHAR* MeshFile;
	const WCHAR* DiffuseTexture;
	const WCHAR* NormalMap;
	const WCHAR* HeightMap;

	Primitives::Type PrimitiveType;
	bool IsPrimitive;
	bool HasMesh;

	bool HasDiffuse;
	bool HasNormal;
	bool HasHeight;
	bool IsEmmissive;
	sm::Vector3 EmmissiveColor;
};


class GameObject
{
public:
	GameObject();
	GameObject(const GODesc& desc);
	~GameObject();

	void InitMesh(const char* objFile);
	void InitMesh(const void* vertices, const void * indices, UINT vertexTypeSize, UINT vertexByteWidth, UINT indexByteWidth, UINT indicesCount);
	
	void SetTexture(const wchar_t* diffuse, const wchar_t* normal, const wchar_t* height);
	void SetTexture(const wchar_t* diffuse);

	const sm::Matrix& GetWorldTransform() { return m_worldTransform; }

	ComPtr<ID3D11ShaderResourceView> GetDiffuseSRV() { return m_textureDiffRV; }
	ComPtr<ID3D11ShaderResourceView> GetNormalSRV()  { return m_textureNormRV; }
	ComPtr<ID3D11ShaderResourceView> GetHeightSRV()  { return m_textureHeightRV; }

	void Set();
	void Update(double dt);
	void Draw();

public:
	sm::Vector3 m_position;
	sm::Vector3 m_rotation;
	sm::Vector3 m_scale;

	ComPtr<ID3D11Buffer> m_materialPropsCB;
	ComPtr<ID3D11Buffer> m_surfacePropsCB;
	MaterialProperties   m_material;
private:
	sm::Matrix           m_worldTransform;

	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;
	Mesh*                m_mesh;

	ComPtr<ID3D11ShaderResourceView> m_textureDiffRV;
	ComPtr<ID3D11ShaderResourceView> m_textureNormRV;
	ComPtr<ID3D11ShaderResourceView> m_textureHeightRV;


	UINT m_indexCount = 0;
	UINT m_stride;
	UINT m_offset = 0;

	bool m_isObj;
};

#define GO_CREATE_MESH(goPtr, mesh) goPtr->InitMesh(mesh::Vertices, mesh::Indices, mesh::VerticesTypeSize, mesh::VerticesByteWidth, mesh::IndicesByteWidth, mesh::IndicesCount)