#pragma once
#include "Core/Core.h"
#include "Core/Structures.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

	void InitMesh(const void* vertices, const void * indices, UINT vertexTypeSize, UINT vertexByteWidth, UINT indexByteWidth, UINT indicesCount);

	void Set();
	void Update(double dt);
	void Draw();

private:
	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;

	MaterialProperties   m_material;
	ComPtr<ID3D11Buffer> m_materialCBuffer;

	ComPtr<ID3D11ShaderResourceView> m_textureRV;

	UINT m_indexCount = 0;
	UINT m_stride;
	UINT m_offset = 0;
};