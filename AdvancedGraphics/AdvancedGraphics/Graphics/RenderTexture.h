#pragma once
#include <Graphics/Shaders.h>

class RenderTarget
{
	friend class Direct3D;
public:
	RenderTarget(UINT width, UINT height);
	~RenderTarget();

	const ComPtr<ID3D11ShaderResourceView>& GetSRV() const { return m_rtSRV; }

private:
	ComPtr<ID3D11Buffer>             m_vertexBuffer;
	ComPtr<ID3D11Buffer>             m_indexBuffer;
	ComPtr<ID3D11Texture2D>          m_renderTargetTexture;
	ComPtr<ID3D11RenderTargetView>   m_renderTargetView;
	ComPtr<ID3D11ShaderResourceView> m_rtSRV;

	VertexShader* m_vertexShader;
	PixelShader* m_pixelShader;

	UINT m_indexCount = 0;
	UINT m_stride;
	UINT m_offset = 0;
};

