#include "pch.h"
#include "GameObject.h"
#include "Utils/Primitives.h"
#include "Core/Texture2D.h"
#include "structures.h"
#include "Direct3D.h"

#define VERTEX_TYPE_SIZE sizeof(SimpleVertex)

GameObject::GameObject(UINT vertexbyteWidth, const void* vertexData, UINT indexByteWidth, const void* indexData)
	:
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_texture(nullptr),
	m_transform(Transform())
{
	D3D->CreateVertexBuffer(m_vertexBuffer, VERTEX_TYPE_SIZE, vertexbyteWidth, vertexData);
	D3D->CreateIndexBuffer(m_indexBuffer, indexByteWidth, indexData);
	D3D->CreateTexture(m_texture, L"Resources\\stone.dds");
}

GameObject::~GameObject()
{
	delete m_vertexBuffer;
	delete m_indexBuffer;
	delete m_texture;
}

void GameObject::Set()
{
	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer->Buffer.GetAddressOf(), &m_vertexBuffer->Stride, &m_vertexBuffer->Offset);
	D3D_CONTEXT->IASetIndexBuffer(m_indexBuffer->Buffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	D3D_CONTEXT->PSSetShaderResources(0, 1, m_texture->ResourceView.GetAddressOf());
	D3D_CONTEXT->PSSetSamplers(0, 1, D3D_DEFAULT_SAMPLER.GetAddressOf());
}

void GameObject::Update(double dt)
{
	this->m_transform.UpdateMatrix();
}

void GameObject::Draw() const noexcept
{
	D3D_CONTEXT->DrawIndexed(Primitives::Cube::IndicesCount, 0, 0);
}
