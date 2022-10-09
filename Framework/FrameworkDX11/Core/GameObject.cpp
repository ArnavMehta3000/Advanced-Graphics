#include "pch.h"
#include "GameObject.h"
#include "Utils/Primitives.h"
#include "Core/Texture2D.h"
#include "Direct3D.h"

#define VERTEX_TYPE_SIZE sizeof(SimpleVertex)

GameObject::GameObject(UINT vertexbyteWidth, const void* vertexData, UINT indexByteWidth, const void* indexData)
	:
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_materialCBuffer(nullptr),
	m_texture(nullptr),
	m_world(sm::Matrix::Identity),
	m_position(sm::Vector3::Zero)
{
	D3D->CreateVertexBuffer(m_vertexBuffer, VERTEX_TYPE_SIZE, vertexbyteWidth, vertexData);
	D3D->CreateIndexBuffer(m_indexBuffer, indexByteWidth, indexData);
	D3D->CreateTexture(m_texture, L"Resources\\stone.dds");

	// Create material constant buffer
	m_materialProps.Material.Diffuse       = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_materialProps.Material.Specular      = XMFLOAT4(1.0f, 0.2f, 0.2f, 1.0f);
	m_materialProps.Material.SpecularPower = 32.0f;
	m_materialProps.Material.UseTexture    = true;

	D3D->CreateConstantBuffer(m_materialCBuffer, sizeof(MaterialPropertiesConstantBuffer));
}

GameObject::~GameObject()
{
	delete m_materialCBuffer;
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
	D3D_CONTEXT->PSSetConstantBuffers(1, 1, m_materialCBuffer->Buffer.GetAddressOf());
	D3D_CONTEXT->UpdateSubresource(m_materialCBuffer->Buffer.Get(), 0, nullptr, &m_materialProps, 0, 0);
}

static float t = 0.0f;
void GameObject::Update([[maybe_unused]]double dt)
{
	t += 0.016;
	sm::Matrix spin = sm::Matrix::CreateRotationY(t);
	sm::Matrix translation = sm::Matrix::CreateTranslation(m_position);
	m_world = translation * spin;
}

void GameObject::Draw() const noexcept
{
	D3D_CONTEXT->DrawIndexed(Primitives::Cube::IndicesCount, 0, 0);
}
