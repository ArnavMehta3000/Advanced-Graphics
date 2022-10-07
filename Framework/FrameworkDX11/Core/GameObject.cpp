#include "pch.h"
#include "GameObject.h"
#include "Utils/Primitives.h"
#include "structures.h"
#include "Direct3D.h"

#define VERTEX_TYPE_SIZE sizeof(SimpleVertex)

GameObject::GameObject(UINT vertexbyteWidth, const void* vertexData, UINT indexByteWidth, const void* indexData)
	:
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_transform(Transform())
{
	D3D->CreateVertexBuffer(m_vertexBuffer, VERTEX_TYPE_SIZE, vertexbyteWidth, vertexData);
	D3D->CreateIndexBuffer(m_indexBuffer, indexByteWidth, indexData);
}

GameObject::~GameObject()
{
	delete m_vertexBuffer;
	delete m_indexBuffer;
}

void GameObject::Set()
{
	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer->Buffer.GetAddressOf(), &m_vertexBuffer->Stride, &m_vertexBuffer->Offset);
	D3D_CONTEXT->IASetIndexBuffer(m_indexBuffer->Buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	D3D_CONTEXT->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void GameObject::Update(double dt)
{
	this->m_transform.UpdateMatrix();
}

void GameObject::Draw() const noexcept
{
}
