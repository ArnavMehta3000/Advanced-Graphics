#pragma once
#include "Core/Core.h"

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

	UINT m_indexCount = 0;
	UINT m_stride;
	UINT m_offset = 0;
};