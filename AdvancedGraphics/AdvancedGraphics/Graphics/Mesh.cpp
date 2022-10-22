#include "pch.h"
#include "Mesh.h"
#include "Direct3D.h"

Mesh::Mesh(std::vector<SimpleVertex>& vertices, std::vector<WORD> indices)
	:
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_useIndexBuffer(true)
{
	CalculateMeshVectors(vertices);

	// Create the vertex buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, vbd);
	vbd.Usage              = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth          = UINT(sizeof(SimpleVertex) * vertices.size());
	vbd.BindFlags          = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags     = 0;

	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vertexInitData);
	vertexInitData.pSysMem = vertices.data();
	HR(D3D_DEVICE->CreateBuffer(&vbd, &vertexInitData, m_vertexBuffer.ReleaseAndGetAddressOf()));


	// Create index buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, ibd);
	ibd.Usage             = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth         = UINT(sizeof(WORD) * indices.size());
	ibd.BindFlags         = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags    = 0;

	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, indexInitData);
	indexInitData.pSysMem = indices.data();
	HR(D3D_DEVICE->CreateBuffer(&ibd, &indexInitData, m_indexBuffer.ReleaseAndGetAddressOf()));

	m_stride       = sizeof(SimpleVertex);
	m_offset       = 0;
	m_indicesCount = (UINT)indices.size();
	m_verticesCount = (UINT)vertices.size();
}

Mesh::Mesh(std::vector<SimpleVertex>& vertices)
	:
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_useIndexBuffer(false)
{
	CalculateMeshVectors(vertices);

	// Create the vertex buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, vbd);
	vbd.Usage          = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth      = UINT(sizeof(SimpleVertex) * vertices.size());
	vbd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

	// Create the index buffer
	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vertexInitData);
	vertexInitData.pSysMem = vertices.data();
	HR(D3D_DEVICE->CreateBuffer(&vbd, &vertexInitData, m_vertexBuffer.ReleaseAndGetAddressOf()));

	m_stride        = sizeof(SimpleVertex);
	m_offset        = 0;
	m_indicesCount  = 0u;
	m_verticesCount = (UINT)vertices.size();
}

Mesh::~Mesh()
{
	COM_RELEASE(m_vertexBuffer);
	COM_RELEASE(m_indexBuffer);
}

void Mesh::CalculateMeshVectors(std::vector<SimpleVertex>& vertices)
{
	int faceCount, index;
	SimpleVertex v1, v2, v3;

	faceCount = (int)vertices.size() / 3;

	// Cycle through every face and get their 3 vertices
	index = 0;
	for (int i = 0; i < faceCount; i++)
	{
		v1.Pos      = vertices[index].Pos;
		v1.TexCoord = vertices[index].TexCoord;
		v1.Normal   = vertices[index].Normal;
		index++;

		v2.Pos      = vertices[index].Pos;
		v2.TexCoord = vertices[index].TexCoord;
		v2.Normal   = vertices[index].Normal;
		index++;

		v3.Pos      = vertices[index].Pos;
		v3.TexCoord = vertices[index].TexCoord;
		v3.Normal   = vertices[index].Normal;
		index++;

		auto [normal, tangent, binormal] = CalculateBinormalTangentLH(v1, v2, v3);

		//vertices[index - 1].Normal   = normal;
		vertices[index - 1].Tangent  = tangent;
		vertices[index - 1].Binormal = binormal;

		//vertices[index - 2].Normal   = normal;
		vertices[index - 2].Tangent  = tangent;
		vertices[index - 2].Binormal = binormal;

		//vertices[index - 3].Normal   = normal;
		vertices[index - 3].Tangent  = tangent;
		vertices[index - 3].Binormal = binormal;
	}
}

Mesh::NormalTangentBinormal Mesh::CalculateBinormalTangentLH(SimpleVertex v1, SimpleVertex v2, SimpleVertex v3)
{
	sm::Vector3 tangent, normal, binormal;

	sm::Vector3 edge1(v2.Pos - v1.Pos);
	sm::Vector3 edge2(v3.Pos - v1.Pos);

	sm::Vector2 deltaUV1 = (v2.TexCoord - v1.TexCoord);
	sm::Vector2 deltaUV2 = (v3.TexCoord - v1.TexCoord);

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

	tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
	tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
	tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
	tangent.Normalize();

	binormal.x = f * (deltaUV1.x * edge2.x - deltaUV2.x * edge1.x);
	binormal.y = f * (deltaUV1.x * edge2.y - deltaUV2.x * edge1.y);
	binormal.z = f * (deltaUV1.x * edge2.z - deltaUV2.x * edge1.z);
	binormal.Normalize();

	sm::Vector3 vv1 = v1.Pos;
	sm::Vector3 vv2 = v2.Pos;
	sm::Vector3 vv3 = v3.Pos;

	sm::Vector3 e1 = vv2 - vv1;
	sm::Vector3 e2 = vv3 - vv1;

	sm::Vector3 cross = e1.Cross(e2);
	cross.Normalize();
	normal = cross;
	
	return std::make_tuple(normal, tangent, binormal);
}

void Mesh::Draw()
{
	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);

	if (m_useIndexBuffer)
	{
		D3D_CONTEXT->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		D3D_CONTEXT->DrawIndexed(m_indicesCount, 0, 0);
	}
	else
		D3D_CONTEXT->Draw(m_verticesCount, 0);
}