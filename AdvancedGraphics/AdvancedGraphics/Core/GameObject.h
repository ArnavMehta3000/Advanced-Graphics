#pragma once
#include "Core/Core.h"
#include "Core/Structures.h"



class GameObject
{
public:
	GameObject();
	~GameObject();

	void InitMesh(const void* vertices, const void * indices, UINT vertexTypeSize, UINT vertexByteWidth, UINT indexByteWidth, UINT indicesCount, const wchar_t* textureFile);
	const sm::Matrix& GetWorldTransform() { return m_worldTransform; }


	void Set();
	void Update(double dt);
	void Draw();

public:
	sm::Vector3 m_position;
	sm::Vector3 m_rotation;
	sm::Vector3 m_scale;

private:
	sm::Matrix m_worldTransform;

	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;

	MaterialProperties   m_material;
	ComPtr<ID3D11Buffer> m_materialCBuffer;

	ComPtr<ID3D11ShaderResourceView> m_textureRV;

	UINT m_indexCount = 0;
	UINT m_stride;
	UINT m_offset = 0;
};

#define GO_CREATE_MESH(goPtr, mesh, tex) goPtr->InitMesh(mesh::Vertices, mesh::Indices, mesh::VerticesTypeSize, mesh::VerticesByteWidth, mesh::IndicesByteWidth, mesh::IndicesCount, tex)