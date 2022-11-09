#pragma once
#include <Graphics/Shaders.h>

class RenderTexture
{
public:
	RenderTexture(UINT width, UINT height);
	~RenderTexture();

	// Sets the textures render target and depth stencils
	void Attach();
	// Draws the full screen quad
	
	void Draw();

	const ComPtr<ID3D11RenderTargetView>&   GetRenderTargetView()     { return m_renderTargetView; }
	const ComPtr<ID3D11DepthStencilView>&   GetDepthStencilView()     { return m_depthStencilView; }
	const ComPtr<ID3D11ShaderResourceView>& GetRTShaderResourceView() { return m_rtSRV; }
	const ComPtr<ID3D11Texture2D>&          GetRenderTexture()        { return m_renderTargetTexture; }
	const ComPtr<ID3D11Texture2D>&          GetDepthTexture()         { return m_depthStencilTexture; }



private:
	void Set();
	void InitMesh();
	void CreateTexture(UINT width, UINT height);

private:
	ComPtr<ID3D11Buffer>           m_vertexBuffer;
	ComPtr<ID3D11Buffer>           m_indexBuffer;

	ComPtr<ID3D11Texture2D>        m_renderTargetTexture;
	ComPtr<ID3D11Texture2D>        m_depthStencilTexture;

	ComPtr<ID3D11DepthStencilView> m_depthStencilView;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	ComPtr<ID3D11ShaderResourceView> m_rtSRV;

	VertexShader* m_vertexShader;
	PixelShader* m_pixelShader;

	UINT m_indexCount = 0;
	UINT m_stride;
	UINT m_offset = 0;
};

