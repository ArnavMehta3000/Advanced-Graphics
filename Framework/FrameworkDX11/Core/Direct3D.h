#pragma once
#include "Defines.h"
#include "Core/Shaders.h"
#include "Core/Buffers.h"

class Texture2D;

struct Direct3DDesc
{
	HWND WindowHandle;
	bool IsVsync;
	D3D_DRIVER_TYPE   DriverType;
	D3D_FEATURE_LEVEL FeatureLevel;
	int VideoMemory;
	char GPU[128];
	int VRAM;
	int RAM;
};


class Direct3D
{
public:
	~Direct3D() {}

	static void Kill();
	static Direct3D* GetInstance();

	bool Init(HWND hwnd, bool vsync);
	void Shutdown();

	void BeginFrame(const std::array<float, 4> clearColor);
	void EndFrame();

	ID3D11Device* GetDevice() { return m_device.Get(); }
	ID3D11DeviceContext* GetContext() { return m_context.Get(); }
	ID3D11SamplerState* GetSamplerState()  { return m_samplerAnisotropicWrap.Get(); }


	void CreateVertexShader(VertexShader*& vs, LPCWSTR srcFile, LPCSTR profile = "vs_4_0", LPCSTR entryPoint = "VS");
	void CreatePixelShader(PixelShader*& ps, LPCWSTR srcFile, LPCSTR profile = "ps_4_0", LPCSTR entryPoint = "PS");

	void CreateVertexBuffer(VertexBuffer*& vb, UINT typeSize, UINT byteWidth, const void* data, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, UINT cpuAccessFlags = 0);
	void CreateIndexBuffer(IndexBuffer*& ib, UINT byteWidth, const void* data, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, UINT cpuAccessFlags = 0);

	void CreateTexture(Texture2D*& tex, const wchar_t* fileName);

private:
	Direct3D();

	static Direct3D* s_instance;

	D3D_DRIVER_TYPE                m_driverType;
	D3D_FEATURE_LEVEL              m_featureLevel;

	ComPtr<ID3D11Device>           m_device;
	ComPtr<ID3D11DeviceContext>    m_context;
	ComPtr<IDXGISwapChain>         m_swapChain;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	
	ComPtr<ID3D11Texture2D>        m_depthStencilTexture;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	ComPtr<ID3D11SamplerState>     m_samplerAnisotropicWrap;

	Direct3DDesc                   m_d3dDesc;
	HWND                           m_hWnd;
	bool                           m_vsync;
};

#define D3D Direct3D::GetInstance()
#define D3D_CONTEXT Direct3D::GetInstance()->GetContext()
#define D3D_DEVICE Direct3D::GetInstance()->GetDevice()
#define D3D_DEFAULT_SAMPLER Direct3D::GetInstance()->GetSamplerState()