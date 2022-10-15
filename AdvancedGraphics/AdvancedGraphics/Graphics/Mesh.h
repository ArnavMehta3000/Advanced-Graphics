#pragma once
#include "Core/Core.h"
#include "Core/Structures.h"

class Mesh
{
public:
	Mesh(std::vector<SimpleVertex>& vertices, std::vector<WORD> indices);
	Mesh(std::vector<SimpleVertex>& vertices);
	~Mesh();

	void Draw();

private:
	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;
	UINT m_stride;
	UINT m_offset;
	UINT m_indicesCount;
	UINT m_verticesCount;

	bool m_useIndexBuffer;
};