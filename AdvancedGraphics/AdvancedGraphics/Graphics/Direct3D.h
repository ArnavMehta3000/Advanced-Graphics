#include "Core/Structures.h"
#include "Graphics/Shaders.h"
#include "Graphics/Targets.h"


constexpr UINT G_BUFFER_COUNT = 4u;
// 0 - Diffuse
// 1 - Normal
// 2 - Position
// 3 - Accumulation
// ---------------
// (ext)RT-0: Accumulation with post process

class Direct3D
{
public:
	static void Kill();  // Used to delete the singleton instance
	static Direct3D* GetInstance();

	bool Init(HWND hwnd, bool isVsync, UINT msaa = 1);
	void CreateDepthStencilStates();
	void Shutdown();
	void EndFrame();

	inline ID3D11Device*              GetDevice()             { return m_device.Get(); }
	inline ID3D11DeviceContext*       GetContext()            { return m_context.Get(); }

	DXGI_FORMAT GetBackBufferFormat() { return m_backBufferFormat; }

	void BindBackBuffer(bool bindDSV = false);
	void UnbindAllTargetsAndResources();

	void SetWireframe(bool isWireframe) { m_context->RSSetState((isWireframe) ? m_rasterWireframe.Get() : m_rasterSolid.Get()); }

	/// <param name="cullBack">True - solid | False - Cull None</param>
	void SetCullMode(bool cullBack) { m_context->RSSetState((cullBack) ? m_rasterSolid.Get() : m_rasterCullNone.Get()); }

	void CreateVertexShader(VertexShader*& vs, LPCWSTR srcFile, LPCSTR profile = "vs_5_0", LPCSTR entryPoint = "VS");
	void CreatePixelShader(PixelShader*& ps, LPCWSTR srcFile, LPCSTR profile = "ps_5_0", LPCSTR entryPoint = "PS");

	void CreateConstantBuffer(ComPtr<ID3D11Buffer>& buf, UINT size, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, UINT cpuAccess = 0);

public:
	DepthTarget                      m_depthTarget;
	ComPtr<ID3D11SamplerState>       m_samplerAnisotropicWrap;
	ComPtr<ID3D11DepthStencilState>  m_depthWriteState;
	ComPtr<ID3D11DepthStencilState>  m_depthReadState;
	ComPtr<ID3D11DepthStencilState>  m_depthStateDefault;

private:
	Direct3D();

	static Direct3D* s_instance;

	ComPtr<ID3D11Device>           m_device;
	ComPtr<ID3D11DeviceContext>    m_context;
	ComPtr<IDXGISwapChain>         m_swapChain;
	ComPtr<ID3D11RenderTargetView> m_backBufferRTV;
								   
	ComPtr<ID3D11BlendState>       m_blendState;
								   
								   
	ComPtr<ID3D11RasterizerState>  m_rasterWireframe;
	ComPtr<ID3D11RasterizerState>  m_rasterSolid;
	ComPtr<ID3D11RasterizerState>  m_rasterCullNone;

private:
	HWND m_hWnd;
	bool m_isVsync;
	DXGI_FORMAT m_backBufferFormat;
	UINT RAM, VRAM;
	char GPU[128];
};

#define D3D	Direct3D::GetInstance()
#define D3D_CONTEXT Direct3D::GetInstance()->GetContext()
#define D3D_DEVICE Direct3D::GetInstance()->GetDevice()
