#pragma once
#include <Graphics/Shaders.h>

class RenderTexture
{
public:
	RenderTexture(UINT width, UINT height);
	~RenderTexture();

	void Attach();

private:
	void InitMesh();
	void CreateTexture(UINT width, UINT height);

private:
	ComPtr<ID3D11Buffer> m_vertexBuffer;
	ComPtr<ID3D11Buffer> m_indexBuffer;
	ComPtr<ID3D11Texture2D> m_renderTargetTexture;
	
	VertexShader* m_vertexShader;
	PixelShader* m_pixelShader;

	UINT m_indexCount = 0;
	UINT m_stride;
	UINT m_offset = 0;
};

