#pragma once


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

	static Direct3D* GetInstance();

	bool Init(HWND hwnd, bool vsync);
	void Shutdown();

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

	Direct3DDesc                   m_d3dDesc;
	HWND                           m_hWnd;
	bool                           m_vsync;
};

#define D3D Direct3D::GetInstance()