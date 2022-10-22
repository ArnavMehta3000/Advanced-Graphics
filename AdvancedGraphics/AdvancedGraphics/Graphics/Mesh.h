#pragma once
#include "Core/Core.h"
#include "Core/Structures.h"
#include <tuple>

class Mesh
{
typedef std::tuple<sm::Vector3, sm::Vector3, sm::Vector3> NormalTangentBinormal;

public:
	Mesh(std::vector<SimpleVertex>& vertices, std::vector<WORD> indices);
	Mesh(std::vector<SimpleVertex>& vertices);
	~Mesh();

	void Draw();

private:
	void CalculateMeshVectors(std::vector<SimpleVertex>& vertices);
	// Returns a tuple where 0: normal, 1: tangent, 2: binormal
	NormalTangentBinormal CalculateBinormalTangentLH(SimpleVertex v1, SimpleVertex v2, SimpleVertex v3);

private:
	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;
	UINT m_stride;
	UINT m_offset;
	UINT m_indicesCount;
	UINT m_verticesCount;

	bool m_useIndexBuffer;
};