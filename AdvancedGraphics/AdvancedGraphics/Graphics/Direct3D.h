#include "Core/Structures.h"
#include "Graphics/Shaders.h"

class Direct3D
{
public:
	static void Kill();  // Used to delete the singleton instance
	static Direct3D* GetInstance();

	bool Init(HWND hwnd, bool isVsync);
	void Shutdown();

	void BeginFrame(const std::array<float, 4> clearColor);
	void EndFrame();

	inline ID3D11Device*              GetDevice()       { return m_device.Get(); }
	inline ID3D11DeviceContext*       GetContext()      { return m_context.Get(); }
	inline ComPtr<ID3D11SamplerState> GetSamplerState() { return m_samplerAnisotropicWrap; }

	void SetWireframe(bool isWireframe) { m_context->RSSetState((isWireframe) ? m_rasterWireframe.Get() : m_rasterSolid.Get()); }

	void CreateVertexShader(VertexShader*& vs, LPCWSTR srcFile, LPCSTR profile = "vs_5_0", LPCSTR entryPoint = "VS");
	void CreatePixelShader(PixelShader*& ps, LPCWSTR srcFile, LPCSTR profile = "ps_5_0", LPCSTR entryPoint = "PS");

	void CreateConstantBuffer(ComPtr<ID3D11Buffer>& buf, UINT size);

private:
	Direct3D();

	static Direct3D* s_instance;

	ComPtr<ID3D11Device>           m_device;
	ComPtr<ID3D11DeviceContext>    m_context;
	ComPtr<IDXGISwapChain>         m_swapChain;
	ComPtr<ID3D11RenderTargetView> m_renderTargetView;

	ComPtr<ID3D11Texture2D>        m_depthStencilTexture;
	ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	ComPtr<ID3D11SamplerState>     m_samplerAnisotropicWrap;

	ComPtr<ID3D11RasterizerState>  m_rasterWireframe;
	ComPtr<ID3D11RasterizerState>  m_rasterSolid;


	HWND m_hWnd;
	bool m_isVsync;

	UINT RAM, VRAM;
	char GPU[128];
};

#define D3D	Direct3D::GetInstance()
#define D3D_CONTEXT Direct3D::GetInstance()->GetContext()
#define D3D_DEVICE Direct3D::GetInstance()->GetDevice()
#define D3D_DEFAULT_SAMPLER Direct3D::GetInstance()->GetSamplerState()