#include "Core/Structures.h"
#include "Graphics/Shaders.h"
#include "Graphics/RenderTexture.h"


constexpr UINT G_BUFFER_COUNT = 4u;
// 0 - Diffuse
// 1 - Normal
// 2 - Position
// 3 - Depth

class Direct3D
{
public:
	static void Kill();  // Used to delete the singleton instance
	static Direct3D* GetInstance();

	bool Init(HWND hwnd, bool isVsync, UINT msaa = 1);
	void InitGBuffer(UINT width, UINT height);
	void Shutdown();
	void EndFrame();

	inline ID3D11Device*              GetDevice()       { return m_device.Get(); }
	inline ID3D11DeviceContext*       GetContext()      { return m_context.Get(); }
	inline ComPtr<ID3D11SamplerState> GetSamplerState() { return m_samplerAnisotropicWrap; }


	void SetWireframe(bool isWireframe) { m_context->RSSetState((isWireframe) ? m_rasterWireframe.Get() : m_rasterSolid.Get()); }

	void BindGBuffer();
	void BindBackBuffer();
	void UnBindAllRenderTargets();
	void BindRenderTarget(const RenderTarget* rt);

	void DrawFSQuad(const RenderTarget* rt);

	/// <param name="cullBack">True - solid | False - Cull None</param>
	void SetCullMode(bool cullBack) { m_context->RSSetState((cullBack) ? m_rasterSolid.Get() : m_rasterCullNone.Get()); }

	void CreateVertexShader(VertexShader*& vs, LPCWSTR srcFile, LPCSTR profile = "vs_5_0", LPCSTR entryPoint = "VS");
	void CreatePixelShader(PixelShader*& ps, LPCWSTR srcFile, LPCSTR profile = "ps_5_0", LPCSTR entryPoint = "PS");

	void CreateConstantBuffer(ComPtr<ID3D11Buffer>& buf, UINT size, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, UINT cpuAccess = 0);

public:
	sm::Vector4 m_clearColor;

private:
	Direct3D();

	static Direct3D* s_instance;

	ComPtr<ID3D11Device>             m_device;
	ComPtr<ID3D11DeviceContext>      m_context;
	ComPtr<IDXGISwapChain>           m_swapChain;
	ComPtr<ID3D11RenderTargetView>   m_backBufferRTV;

	ComPtr<ID3D11SamplerState>       m_samplerAnisotropicWrap;
	ComPtr<ID3D11BlendState>         m_blendState;

	ComPtr<ID3D11RasterizerState>    m_rasterWireframe;
	ComPtr<ID3D11RasterizerState>    m_rasterSolid;
	ComPtr<ID3D11RasterizerState>    m_rasterCullNone;

public:
	// GBuffer
	ComPtr<ID3D11RenderTargetView>   m_rtvArray[G_BUFFER_COUNT];
	ComPtr<ID3D11ShaderResourceView> m_srvArray[G_BUFFER_COUNT];
	ComPtr<ID3D11Texture2D>          m_textureArray[G_BUFFER_COUNT];
	ComPtr<ID3D11DepthStencilView>   m_depthStencilView;

private:
	HWND m_hWnd;
	bool m_isVsync;

	UINT RAM, VRAM;
	char GPU[128];
};

#define D3D	Direct3D::GetInstance()
#define D3D_CONTEXT Direct3D::GetInstance()->GetContext()
#define D3D_DEVICE Direct3D::GetInstance()->GetDevice()
#define D3D_DEFAULT_SAMPLER Direct3D::GetInstance()->GetSamplerState()