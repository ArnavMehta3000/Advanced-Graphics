#include "pch.h"
#include "GameObject.h"
#include "Graphics/Primitives.h"
#include "Graphics/Direct3D.h"
#include "Utils/DDSTextureLoader.h"

GameObject::GameObject()
	:
	m_stride(0)
{
}

GameObject::~GameObject()
{
	COM_RELEASE(m_textureRV);
	COM_RELEASE(m_vertexBuffer);
	COM_RELEASE(m_indexBuffer);
	COM_RELEASE(m_materialCBuffer);
}

void GameObject::InitMesh(const void* vertices, const void* indices, UINT vertexTypeSize, UINT vertexByteWidth, UINT indexByteWidth, UINT indicesCount)
{
	// Create vertex buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, vbd);
	vbd.Usage             = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth         = vertexByteWidth;
	vbd.BindFlags         = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags    = 0;
	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vertexInitData);
	vertexInitData.pSysMem = vertices;
	HR(D3D_DEVICE->CreateBuffer(&vbd, &vertexInitData, m_vertexBuffer.ReleaseAndGetAddressOf()));

	// Save vertex/index  buffer data
	m_stride     = vertexTypeSize;
	m_offset     = 0;
	m_indexCount = indicesCount;
	
	// Create index buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, ibd);
	ibd.Usage             = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth         = indexByteWidth;
	ibd.BindFlags         = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags    = 0;
	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, indexInitData);
	indexInitData.pSysMem      = indices;	
	HR(D3D_DEVICE->CreateBuffer(&ibd, &indexInitData, m_indexBuffer.ReleaseAndGetAddressOf()));

	// Create material constant buffer
	m_material.Material.Diffuse       = sm::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	m_material.Material.Specular      = sm::Vector4(1.0f, 0.2f, 0.2f, 1.0f);
	m_material.Material.SpecularPower = 32.0f;
	m_material.Material.UseTexture    = true;

	D3D->CreateConstantBuffer(m_materialCBuffer, sizeof(MaterialProperties));

	// Load texture 
	HR(CreateDDSTextureFromFile(D3D_DEVICE, D3D_CONTEXT, L"Assets\\stone.dds", nullptr, m_textureRV.ReleaseAndGetAddressOf()));
}

void GameObject::Set()
{
	D3D_CONTEXT->PSSetSamplers(0, 1, D3D_DEFAULT_SAMPLER.GetAddressOf());
	D3D_CONTEXT->PSSetShaderResources(0, 1, m_textureRV.GetAddressOf());
	D3D_CONTEXT->PSSetConstantBuffers(1, 1, m_materialCBuffer.GetAddressOf());


	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);
	D3D_CONTEXT->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
}

void GameObject::Update(double dt)
{
	D3D_CONTEXT->UpdateSubresource(m_materialCBuffer.Get(), 0, nullptr, &m_material, 0, 0);
}

void GameObject::Draw()
{
	D3D_CONTEXT->DrawIndexed(m_indexCount, 0, 0);
}