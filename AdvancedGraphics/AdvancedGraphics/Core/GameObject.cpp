#include "pch.h"
#include "GameObject.h"
#include "Graphics/Primitives.h"
#include "Graphics/Direct3D.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{

}

void GameObject::InitMesh(const void* vertices, const void* indices, UINT vertexTypeSize, UINT vertexByteWidth, UINT indexByteWidth, UINT indicesCount)
{
	D3D11_BUFFER_DESC vbd = {};
	vbd.Usage             = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth         = vertexByteWidth;
	vbd.BindFlags         = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags    = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = Primitives::Triangle::Vertices;

	m_stride     = vertexTypeSize;
	m_offset     = 0;
	m_indexCount = indicesCount;
	

	HR(D3D_DEVICE->CreateBuffer(&vbd, &InitData, m_vertexBuffer.ReleaseAndGetAddressOf()));

	D3D11_BUFFER_DESC ibd = {};
	ibd.Usage             = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth         = indexByteWidth;
	ibd.BindFlags         = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags    = 0;
	InitData.pSysMem      = indices;
	HR(D3D_DEVICE->CreateBuffer(&ibd, &InitData, m_indexBuffer.ReleaseAndGetAddressOf()));
}

void GameObject::Set()
{
	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);
	D3D_CONTEXT->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
}

void GameObject::Update(double dt)
{
}

void GameObject::Draw()
{
	D3D_CONTEXT->DrawIndexed(m_indexCount, 0, 0);
}