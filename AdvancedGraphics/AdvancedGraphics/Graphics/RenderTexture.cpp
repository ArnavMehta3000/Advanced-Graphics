#include "pch.h"
#include "RenderTexture.h"
#include "Graphics/Direct3D.h"
#include "Graphics/Primitives.h"

RenderTexture::RenderTexture(UINT width, UINT height)
	:
	m_vertexBuffer(nullptr),
	m_indexBuffer(nullptr),
	m_vertexShader(nullptr),
	m_pixelShader(nullptr),
	m_renderTargetTexture(nullptr),
	m_renderTargetView(nullptr),
	m_depthStencilTexture(nullptr),
	m_depthStencilView(nullptr),
	m_rtSRV(nullptr)
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
	COM_RELEASE(m_depthStencilTexture);

	COM_RELEASE(m_renderTargetView);
	COM_RELEASE(m_depthStencilView);

	COM_RELEASE(m_rtSRV);
}

void RenderTexture::Attach()
{
	D3D->SetRenderAndDepthTargets(m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());
	D3D->BeginFrame({ 0.01f, 0.01f, 0.01f, 1.0f }, m_renderTargetView.Get(), m_depthStencilView.Get());
}


void RenderTexture::InitMesh()
{
	// Create vertex buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, vbd);
	vbd.Usage              = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth          = Primitives::FSQuad::VerticesByteWidth;
	vbd.BindFlags          = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags     = 0;
	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vertexInitData);
	vertexInitData.pSysMem = Primitives::FSQuad::Vertices;
	HR(D3D_DEVICE->CreateBuffer(&vbd, &vertexInitData, m_vertexBuffer.ReleaseAndGetAddressOf()));

	// Save vertex/index  buffer data
	m_stride = Primitives::FSQuad::VerticesTypeSize;
	m_offset = 0;
	m_indexCount = Primitives::FSQuad::IndicesCount;

	// Create index buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, ibd);
	ibd.Usage             = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth         = Primitives::FSQuad::IndicesByteWidth;
	ibd.BindFlags         = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags    = 0;
	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, indexInitData);
	indexInitData.pSysMem = Primitives::FSQuad::Indices;
	HR(D3D_DEVICE->CreateBuffer(&ibd, &indexInitData, m_indexBuffer.ReleaseAndGetAddressOf()));
}

void RenderTexture::CreateTexture(UINT width, UINT height)
{
	// Create render target view
	CREATE_ZERO(D3D11_TEXTURE2D_DESC, texDesc);
	{
		texDesc.Width              = width;
		texDesc.Height             = height;
		texDesc.MipLevels          = 1;
		texDesc.ArraySize          = 1;
		texDesc.Format             = DXGI_FORMAT_R32G32B32A32_FLOAT;
		texDesc.SampleDesc.Count   = 1;
		texDesc.Usage              = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags          = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		texDesc.CPUAccessFlags     = 0;
		texDesc.MiscFlags          = 0;
		HR(D3D_DEVICE->CreateTexture2D(&texDesc, NULL, m_renderTargetTexture.ReleaseAndGetAddressOf()));

		CREATE_ZERO(D3D11_RENDER_TARGET_VIEW_DESC, rtvDesc);
		rtvDesc.Format = texDesc.Format;
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		rtvDesc.Texture2D.MipSlice = 0;
		HR(D3D_DEVICE->CreateRenderTargetView(m_renderTargetTexture.Get(), &rtvDesc, m_renderTargetView.ReleaseAndGetAddressOf()));
	}
	
	// Create depth stencil view
	CREATE_ZERO(D3D11_TEXTURE2D_DESC, dstDesc);
	{
		dstDesc.Width              = width;
		dstDesc.Height             = height;
		dstDesc.MipLevels          = 1;
		dstDesc.ArraySize          = 1;
		dstDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dstDesc.SampleDesc.Count   = 1;
		dstDesc.SampleDesc.Quality = 0;
		dstDesc.Usage              = D3D11_USAGE_DEFAULT;
		dstDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
		dstDesc.CPUAccessFlags     = 0;
		dstDesc.MiscFlags          = 0;
		HR(D3D_DEVICE->CreateTexture2D(&dstDesc, nullptr, m_depthStencilTexture.ReleaseAndGetAddressOf()));

		CREATE_ZERO(D3D11_DEPTH_STENCIL_VIEW_DESC, dsvDesc);
		dsvDesc.Format             = dstDesc.Format;
		dsvDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		dsvDesc.Texture2D.MipSlice = 0;
		HR(D3D_DEVICE->CreateDepthStencilView(m_depthStencilTexture.Get(), &dsvDesc, m_depthStencilView.ReleaseAndGetAddressOf()));
	}

	// Create render target's shader resource view
	CREATE_ZERO(D3D11_SHADER_RESOURCE_VIEW_DESC, srvDesc);
	{
		srvDesc.Format                    = texDesc.Format;
		srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels       = 1;
		HR(D3D_DEVICE->CreateShaderResourceView(m_renderTargetTexture.Get(), &srvDesc, m_rtSRV.ReleaseAndGetAddressOf()));
	}

}

void RenderTexture::Set()
{
}

void RenderTexture::Draw()
{
	D3D_CONTEXT->IASetInputLayout(m_vertexShader->InputLayout.Get());
	D3D_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D_CONTEXT->VSSetShader(m_vertexShader->Shader.Get(), nullptr, 0);
	D3D_CONTEXT->PSSetShader(m_pixelShader->Shader.Get(), nullptr, 0);

	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_stride, &m_offset);
	D3D_CONTEXT->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// Set pixel shader texture and sampler
	D3D_CONTEXT->PSSetSamplers(0, 1, D3D_DEFAULT_SAMPLER.GetAddressOf());
	D3D_CONTEXT->PSSetShaderResources(0, 1, m_rtSRV.GetAddressOf());

	D3D_CONTEXT->DrawIndexed(m_indexCount, 0, 0);
}
