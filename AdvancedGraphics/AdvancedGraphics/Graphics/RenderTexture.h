#pragma once
#include <Graphics/Shaders.h>

class RenderTarget
{
	friend class Direct3D;
public:
	RenderTarget(UINT width, UINT height);
	~RenderTarget();

	const ComPtr<ID3D11ShaderResourceView>& GetSRV() const { return m_rtSRV; }

	float GetWidth()  const noexcept { return m_width; }
	float GetHeight() const noexcept { return m_height; }

	void InitMesh();
	void CreateTexture(UINT width, UINT height);

private:
	ComPtr<ID3D11Buffer>             m_vertexBuffer;
	ComPtr<ID3D11Buffer>             m_indexBuffer;
	ComPtr<ID3D11Texture2D>          m_renderTargetTexture;
	ComPtr<ID3D11RenderTargetView>   m_renderTargetView;
	ComPtr<ID3D11ShaderResourceView> m_rtSRV;

	VertexShader* m_vertexShader;
	PixelShader* m_pixelShader;

	float m_width, m_height;

	UINT m_indexCount = 0;
	UINT m_stride;
	UINT m_offset = 0;
};

