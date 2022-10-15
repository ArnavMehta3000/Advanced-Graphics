#include "pch.h"
#include "Mesh.h"
#include "Direct3D.h"

Mesh::Mesh(std::vector<SimpleVertex>& vertices, std::vector<WORD> indices)
	:
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_useIndexBuffer(true)
{
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
	// Create the vertex buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, vbd);
	vbd.Usage          = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth      = UINT(sizeof(SimpleVertex) * vertices.size());
	vbd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;

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