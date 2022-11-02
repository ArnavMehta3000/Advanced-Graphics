#include "pch.h"
#include "RenderTexture.h"
#include "Graphics/Direct3D.h"
#include "Graphics/Primitives.h"

RenderTexture::RenderTexture(UINT width, UINT height)
	:
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_vertexShader(nullptr),
	m_pixelShader(nullptr)
{
	// Create shaders
	D3D->CreateVertexShader(m_vertexShader, L"Shaders/RenderTexture.fx");
	D3D->CreatePixelShader(m_pixelShader, L"Shaders/RenderTexture.fx");

	CreateTexture(width, height);	
	InitMesh();
}

RenderTexture::~RenderTexture()
{
	SAFE_DELETE(m_vertexShader);
	SAFE_DELETE(m_pixelShader);
	COM_RELEASE(m_vertexBuffer);
	COM_RELEASE(m_indexBuffer);
	COM_RELEASE(m_renderTargetTexture);
}

void RenderTexture::Attach()
{
	//TODO: Finish attach function
	//D3D_CONTEXT->OMSetRenderTargets(1)
}

void RenderTexture::InitMesh()
{
	// Create vertex buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, vbd);
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = Primitives::FSQuad::VerticesByteWidth;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vertexInitData);
	vertexInitData.pSysMem = Primitives::FSQuad::Vertices;
	HR(D3D_DEVICE->CreateBuffer(&vbd, &vertexInitData, m_vertexBuffer.ReleaseAndGetAddressOf()));

	// Save vertex/index  buffer data
	m_stride = Primitives::FSQuad::VerticesTypeSize;
	m_offset = 0;
	m_indexCount = Primitives::FSQuad::IndicesCount;

	// Create index buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, ibd);
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = Primitives::FSQuad::IndicesByteWidth;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, indexInitData);
	indexInitData.pSysMem = Primitives::FSQuad::Indices;
	HR(D3D_DEVICE->CreateBuffer(&ibd, &indexInitData, m_indexBuffer.ReleaseAndGetAddressOf()));
}

void RenderTexture::CreateTexture(UINT width, UINT height)
{
	CREATE_ZERO(D3D11_TEXTURE2D_DESC, texDesc);
	texDesc.Width = width;
	texDesc.Height = height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	D3D_DEVICE->CreateTexture2D(&texDesc, nullptr, m_renderTargetTexture.ReleaseAndGetAddressOf());
}
