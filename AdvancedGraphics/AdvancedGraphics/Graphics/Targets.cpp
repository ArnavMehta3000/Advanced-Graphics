#include "pch.h"
#include "Graphics/Targets.h"
#include "Graphics/Direct3D.h"
#include "Core/Core.h"


Target::Target(DXGI_FORMAT textureFormat, DXGI_FORMAT srvFormat, UINT width, UINT height, UINT flag)
	:
	m_width(width),
	m_height(height),
	m_textureFormat(textureFormat),
	m_srvFormat(srvFormat),
	m_bindFlags(flag),
	m_srv(nullptr),
	m_texture(nullptr)
{
	// Create texture
	CREATE_ZERO(D3D11_TEXTURE2D_DESC, texDesc);
	{
		texDesc.Width              = m_width;
		texDesc.Height             = m_height;
		texDesc.MipLevels          = 1;
		texDesc.ArraySize          = 1;
		texDesc.Format             = m_textureFormat;
		texDesc.SampleDesc.Count   = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Usage              = D3D11_USAGE_DEFAULT;
		texDesc.BindFlags          = m_bindFlags;
		texDesc.CPUAccessFlags     = 0;
		texDesc.MiscFlags          = 0;
	}
	HR(D3D_DEVICE->CreateTexture2D(&texDesc, NULL, m_texture.ReleaseAndGetAddressOf()));

	// Create shader resource view
	CREATE_ZERO(D3D11_SHADER_RESOURCE_VIEW_DESC, srDesc);
	{
		srDesc.Format                    = m_srvFormat;
		srDesc.ViewDimension             = D3D_SRV_DIMENSION_TEXTURE2D;
		srDesc.Texture2D.MostDetailedMip = 0;
		srDesc.Texture2D.MipLevels       = 1;
	}
	HR(D3D_DEVICE->CreateShaderResourceView(m_texture.Get(), &srDesc, m_srv.ReleaseAndGetAddressOf()));
}

void Target::Release()
{
	COM_RELEASE(m_srv);
	COM_RELEASE(m_texture);
}


RenderTarget::RenderTarget(DXGI_FORMAT textureFormat, UINT width, UINT height, UINT flag)
	:
	Target(textureFormat, textureFormat, width, height, flag),
	m_rtv(nullptr)	
{
	// Create render target view
	CREATE_ZERO(D3D11_RENDER_TARGET_VIEW_DESC, rtDesc);
	{
		rtDesc.Format        = m_textureFormat;
		rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	}
	HR(D3D_DEVICE->CreateRenderTargetView(m_texture.Get(), &rtDesc, m_rtv.ReleaseAndGetAddressOf()));
}

RenderTarget::~RenderTarget()
{
	this->Release();
}

void RenderTarget::Release()
{
	Target::Release();
	COM_RELEASE(m_rtv);
}



DepthTarget::DepthTarget(DXGI_FORMAT textureFormat, DXGI_FORMAT stencilFormat, DXGI_FORMAT srvFormat, UINT width, UINT height, UINT flag)
	:
	Target(textureFormat, srvFormat, width, height, flag),
	m_stencilFormat(stencilFormat),
	m_dsv(nullptr)
{
	// Create depth stencil view
	CREATE_ZERO(D3D11_DEPTH_STENCIL_VIEW_DESC, dsDesc);
	{
		dsDesc.Flags         = 0;
		dsDesc.Format        = stencilFormat;
		dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	}
	HR(D3D_DEVICE->CreateDepthStencilView(m_texture.Get(), &dsDesc, m_dsv.ReleaseAndGetAddressOf()));
}

DepthTarget::~DepthTarget()
{
	this->Release();
}

void DepthTarget::Release()
{
	COM_RELEASE(m_dsv);
	Target::Release();
}
