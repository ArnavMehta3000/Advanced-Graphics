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
	void Set();
	void Draw();

	ID3D11RenderTargetView* GetRenderTargetView() { return m_renderTargetView.Get(); }
	ID3D11ShaderResourceView* GetRTShaderResourceView() { return m_rtSRV.Get(); }


private:
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

