#pragma once

class Target
{
public:
	Target() = default;
	Target(DXGI_FORMAT textureFormat, DXGI_FORMAT srvFormat, UINT width, UINT height, UINT flag);
	virtual ~Target() = default;

	virtual void Release();

	UINT        GetWdith()         { return m_width; }
	UINT        GetHeight()        { return m_height; }
	DXGI_FORMAT GetTextureFormat() { return m_textureFormat; }
	DXGI_FORMAT GetSRVFormat()     { return m_srvFormat; }

	inline ComPtr<ID3D11ShaderResourceView>& SRV()     { return m_srv; }
	inline ComPtr<ID3D11Texture2D>&          Texture() { return m_texture; }

protected:
	UINT            m_width;
	UINT            m_height;
	UINT            m_bindFlags;
	DXGI_FORMAT     m_textureFormat;
	DXGI_FORMAT     m_srvFormat;

	ComPtr<ID3D11ShaderResourceView> m_srv;
	ComPtr<ID3D11Texture2D>          m_texture;
};




class RenderTarget : public Target
{
public:
	RenderTarget() = default;
	RenderTarget(DXGI_FORMAT textureFormat, UINT width, UINT height, UINT flag);
	~RenderTarget();

	void Release() override;
	inline ComPtr<ID3D11RenderTargetView>&   RTV()     { return m_rtv; }

private:
	ComPtr<ID3D11RenderTargetView>   m_rtv;
};




class DepthTarget : public Target
{
public:
	DepthTarget() = default;
	DepthTarget(DXGI_FORMAT textureFormat, DXGI_FORMAT stencilFormat, DXGI_FORMAT srvFormat, UINT width, UINT height, UINT flag);
	~DepthTarget();

	inline DXGI_FORMAT GetDSVFormat() { return m_srvFormat; }

	void Release() override;
	inline ComPtr<ID3D11DepthStencilView>& DSV() { return m_dsv; }

private:
	DXGI_FORMAT m_stencilFormat;
	ComPtr<ID3D11DepthStencilView> m_dsv;
};