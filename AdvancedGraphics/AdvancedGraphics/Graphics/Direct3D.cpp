#include "pch.h"
#include "Direct3D.h"
#include "Core/Core.h"
#include "Core/Structures.h"

Direct3D* Direct3D::s_instance = nullptr;

Direct3D::Direct3D()
	:
	m_device(nullptr),
	m_context(nullptr),
	m_swapChain(nullptr),
	m_backBufferRTV(nullptr),
	m_samplerAnisotropicWrap(nullptr),
	m_blendState(nullptr),
	m_rasterWireframe(nullptr),
	m_rasterSolid(nullptr),
	m_rasterCullNone(nullptr)
{}

void Direct3D::Kill()
{
	delete s_instance;
	s_instance = nullptr;
}

Direct3D* Direct3D::GetInstance()
{
	if (s_instance == nullptr)
		s_instance = new Direct3D();

	return s_instance;
}

bool Direct3D::Init(HWND hwnd, bool isVsync, UINT msaa)
{
	m_hWnd = hwnd;
	m_isVsync = isVsync;

	// Get window client rect
	CREATE_ZERO(RECT, rc);
	GetClientRect(m_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;


	// Core initialize
	{
		ComPtr<IDXGIFactory> factory;
		ComPtr<IDXGIAdapter> adapter;
		ComPtr<IDXGIOutput> adapterOutput;

		DXGI_MODE_DESC* displayModeList;
		DXGI_ADAPTER_DESC adapterDesc;
		UINT numModes = 0, numerator = 0, denominator = 1;

		// Create factory and get display modes
		HR(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory));
		HR(factory->EnumAdapters(0, &adapter));
		HR(adapter->EnumOutputs(0, &adapterOutput));
		HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL));

		// Get display modes
		displayModeList = new DXGI_MODE_DESC[numModes];
		HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList));

		// Get right display mode from display mode list
		for (unsigned int i = 0; i < numModes; i++)
		{
			if (displayModeList[i].Width == (unsigned int)width)
			{
				if (displayModeList[i].Height == (unsigned int)height)
				{
					numerator   = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}
		}
		HR(adapter->GetDesc(&adapterDesc));

		// Store memory info (in mb)
		VRAM = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
		RAM  = (int)(adapterDesc.SharedSystemMemory / 1024 / 1024);
		// Store the gpu name
		size_t strLength;
		wcstombs_s(&strLength, GPU, 128, adapterDesc.Description, 128);

		// Cleanup 1
		delete[] displayModeList;
		displayModeList = 0;
		COM_RELEASE(adapter);
		COM_RELEASE(adapterOutput);


		// Create swapchain description
		CREATE_ZERO(DXGI_SWAP_CHAIN_DESC, swapChainDesc);
		swapChainDesc.BufferCount                            = 1;
		swapChainDesc.BufferDesc.Width                       = width;
		swapChainDesc.BufferDesc.Height                      = height;
		swapChainDesc.BufferDesc.Format                      = DXGI_FORMAT_R16G16B16A16_FLOAT;
		if (isVsync)
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator   = numerator;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		}
		else
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator   = 60;  // Lock swapchain refresh rate to 60hz
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}
		swapChainDesc.BufferUsage                            = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
		swapChainDesc.OutputWindow                           = hwnd;
		swapChainDesc.SampleDesc.Count                       = 1;  // Anti aliasing here
		swapChainDesc.SampleDesc.Quality                     = 0;
		swapChainDesc.Windowed                               = TRUE;
		swapChainDesc.BufferDesc.ScanlineOrdering            = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling                     = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SwapEffect                             = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags                                  = 0;

		auto driverType = D3D_DRIVER_TYPE_HARDWARE;
		auto featureLevel = D3D_FEATURE_LEVEL_11_1;
		UINT flags = 0;
#ifdef _DEBUG
		flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG


		// Create device / swapchain / immediate context
		HR(D3D11CreateDeviceAndSwapChain(
			nullptr,
			driverType,
			NULL,
			flags,
			&featureLevel, 1,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			m_swapChain.ReleaseAndGetAddressOf(),
			m_device.ReleaseAndGetAddressOf(),
			NULL,
			m_context.ReleaseAndGetAddressOf())
		);

		// Prevent Alt+Enter fullscreen
		factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
		COM_RELEASE(factory);
	}

	// Create render target view using back buffer as texture resource
	ComPtr<ID3D11Texture2D> backBufferPtr;
	HR(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBufferPtr.ReleaseAndGetAddressOf())));
	HR(m_device->CreateRenderTargetView(backBufferPtr.Get(), nullptr, m_backBufferRTV.ReleaseAndGetAddressOf()));
	COM_RELEASE(backBufferPtr);


	// Create viewport
	CREATE_ZERO(D3D11_VIEWPORT, vp);
	vp.Width    = (FLOAT)width;
	vp.Height   = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_context->RSSetViewports(1, &vp);

	// Create sampler
	CREATE_ZERO(D3D11_SAMPLER_DESC, sampDesc);
	sampDesc.Filter         = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU       = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV       = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW       = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD         = 0;
	sampDesc.MaxLOD         = D3D11_FLOAT32_MAX;
	sampDesc.MaxAnisotropy  = 4;
	HR(m_device->CreateSamplerState(&sampDesc, m_samplerAnisotropicWrap.ReleaseAndGetAddressOf()));

	// Create rasterizer states
	{
		CREATE_ZERO(D3D11_RASTERIZER_DESC, rasterDesc);
		rasterDesc.MultisampleEnable     = TRUE;
		rasterDesc.AntialiasedLineEnable = TRUE;
		rasterDesc.FillMode              = D3D11_FILL_WIREFRAME;
		rasterDesc.CullMode              = D3D11_CULL_NONE;
		HR(m_device->CreateRasterizerState(&rasterDesc, m_rasterWireframe.ReleaseAndGetAddressOf()));

		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		HR(m_device->CreateRasterizerState(&rasterDesc, m_rasterSolid.ReleaseAndGetAddressOf()));

		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.CullMode = D3D11_CULL_NONE;
		HR(m_device->CreateRasterizerState(&rasterDesc, m_rasterCullNone.ReleaseAndGetAddressOf()));
	}

	// Create blend state
	{
		CREATE_ZERO(D3D11_BLEND_DESC, blendDesc);
		blendDesc.AlphaToCoverageEnable                 = false;
		blendDesc.IndependentBlendEnable                = false;
		blendDesc.RenderTarget[0].BlendEnable           = true;
		blendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		HR(m_device->CreateBlendState(&blendDesc, m_blendState.ReleaseAndGetAddressOf()));
	}

	InitGBuffer(width, height);

	SetWireframe(false);

	return true;
}

void Direct3D::InitGBuffer(UINT width, UINT height)
{
	// Create texture array
	CREATE_ZERO(D3D11_TEXTURE2D_DESC, textureDesc);
	textureDesc.Width            = width;
	textureDesc.Height           = height;
	textureDesc.MipLevels        = 1;
	textureDesc.ArraySize        = 1;
	textureDesc.Format           = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage            = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags        = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	for (size_t i = 0; i < G_BUFFER_COUNT; i++)
		HR(m_device->CreateTexture2D(&textureDesc, NULL, m_textureArray[i].ReleaseAndGetAddressOf()));


	// Create render target array
	CREATE_ZERO(D3D11_RENDER_TARGET_VIEW_DESC, renderTargetViewDesc);
	renderTargetViewDesc.Format        = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	for (size_t i = 0; i < G_BUFFER_COUNT; i++)
		HR(m_device->CreateRenderTargetView(m_textureArray[i].Get(), &renderTargetViewDesc, m_rtvArray[i].ReleaseAndGetAddressOf()));


	// Create shader resource views
	CREATE_ZERO(D3D11_SHADER_RESOURCE_VIEW_DESC, shaderResourceViewDesc);
	shaderResourceViewDesc.Format                    = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels       = 1;
	for (size_t i = 0; i < G_BUFFER_COUNT; i++)
		HR(m_device->CreateShaderResourceView(m_textureArray[i].Get(), &shaderResourceViewDesc, m_srvArray[i].ReleaseAndGetAddressOf()));




	// Create depth stencil buffer
	ComPtr<ID3D11Texture2D> depthStencilTexture = nullptr;
	CREATE_ZERO(D3D11_TEXTURE2D_DESC, depthStencilTexDesc);
	depthStencilTexDesc.Width                = width;
	depthStencilTexDesc.Height               = height;
	depthStencilTexDesc.MipLevels            = 1;
	depthStencilTexDesc.ArraySize            = 1;
	depthStencilTexDesc.Format               = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilTexDesc.SampleDesc.Count     = 1;
	depthStencilTexDesc.SampleDesc.Quality   = 0;
	depthStencilTexDesc.Usage                = D3D11_USAGE_DEFAULT;
	depthStencilTexDesc.BindFlags            = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	HR(m_device->CreateTexture2D(&depthStencilTexDesc, NULL, &depthStencilTexture));

	CREATE_ZERO(D3D11_DEPTH_STENCIL_VIEW_DESC, depthStencilViewDesc);
	depthStencilViewDesc.Flags         = 0;
	depthStencilViewDesc.Format        = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	HR(m_device->CreateDepthStencilView(depthStencilTexture.Get(), &depthStencilViewDesc, m_depthStencilView.ReleaseAndGetAddressOf()));

	CREATE_ZERO(D3D11_SHADER_RESOURCE_VIEW_DESC, depthSRV);
	depthSRV.Format              = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	depthSRV.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
	depthSRV.Texture2D.MipLevels = 1;
	HR(m_device->CreateShaderResourceView(depthStencilTexture.Get(), &depthSRV, m_depthSRV.ReleaseAndGetAddressOf()));

	COM_RELEASE(depthStencilTexture);


	m_deferredShader = Shader(L"Shaders/Advanced/DeferredVS.hlsl", L"Shaders/Advanced/DeferredPS.hlsl");

	LOG("Created G-Buffer");
}

void Direct3D::Shutdown()
{
	COM_RELEASE(m_samplerAnisotropicWrap);
	COM_RELEASE(m_rasterSolid);
	COM_RELEASE(m_rasterCullNone);
	COM_RELEASE(m_rasterWireframe);
	COM_RELEASE(m_backBufferRTV);
	COM_RELEASE(m_context);
	COM_RELEASE(m_swapChain);
	COM_RELEASE(m_device);
}

void Direct3D::EndFrame()
{
	if (m_isVsync)
		m_swapChain->Present(1, 0);  // Lock present to refresh rate
	else
		m_swapChain->Present(0, 0);  // Present as fast as swap chain can
}



void Direct3D::BindGBuffer()
{
	// Get rtv array
	ID3D11RenderTargetView* renderTargets[] = { m_rtvArray[0].Get(), m_rtvArray[1].Get() , m_rtvArray[2].Get() };

	// Clear rtv and depth
	for (size_t i = 0; i < G_BUFFER_COUNT; i++)
		m_context->ClearRenderTargetView(m_rtvArray[i].Get(), Colors::Cyan);
	m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1u, 0u);
	m_context->OMSetRenderTargets(_countof(renderTargets), renderTargets, m_depthStencilView.Get());

	m_deferredShader.BindShader();
}

void Direct3D::BindBackBuffer()
{
	m_context->ClearRenderTargetView(m_backBufferRTV.Get(), Colors::HotPink);
	m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1u, 0u);

	m_context->OMSetRenderTargets(1, m_backBufferRTV.GetAddressOf(), nullptr);
}

void Direct3D::BindRenderTarget(const RenderTarget* rt)
{
	m_context->ClearRenderTargetView(rt->m_renderTargetView.Get(), Colors::Green);
	m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1u, 0u);

	m_context->OMSetRenderTargets(1, rt->m_renderTargetView.GetAddressOf(), nullptr);
}

void Direct3D::SetLightingPrePassResources(const RenderTarget* rt)
{
	// Set g buffer as texture resources for PS
	ID3D11ShaderResourceView* rtSRVs[]{ m_srvArray[0].Get(), m_srvArray[2].Get() , m_srvArray[2].Get() };
	m_context->PSSetShaderResources(0, _countof(rtSRVs), rtSRVs);

	// Clear Set render target texture to slot 0
	m_context->ClearRenderTargetView(rt->m_renderTargetView.Get(), Colors::Black);
	m_context->OMSetRenderTargets(1, rt->m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

}

void Direct3D::UnbindAllResourcesAndTargets()
{
	m_context->OMSetRenderTargets(0, nullptr, nullptr);

	// Clear PS shader resource views
	ID3D11ShaderResourceView* srv[5] = { nullptr, nullptr, nullptr, nullptr, nullptr };
	m_context->PSSetShaderResources(0, _countof(srv), srv);
}


void Direct3D::DrawFSQuad(const RenderTarget* rt)
{
	// Set shader data
	m_context->VSSetShader(rt->m_vertexShader->Shader.Get(), nullptr, 0);
	//m_context->PSSetShader(rt->m_pixelShader->Shader.Get(), nullptr, 0);

	// Set texture
	//m_context->PSSetShaderResources(0, 1, rt->GetSRV().GetAddressOf());

	// Set sampler
	m_context->PSSetSamplers(0, 1, D3D_SAMPLER_LINEAR.GetAddressOf());
	
	// Set buffers
	m_context->IASetVertexBuffers(0, 1,rt-> m_vertexBuffer.GetAddressOf(), &rt->m_stride, &rt->m_offset);
	m_context->IASetIndexBuffer(rt->m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	
	m_context->IASetInputLayout(rt->m_vertexShader->InputLayout.Get());

	// Draw the quad
	m_context->DrawIndexed(rt->m_indexCount, 0, 0);
}

void Direct3D::CreateVertexShader(VertexShader*& vs, LPCWSTR srcFile, LPCSTR profile, LPCSTR entryPoint)
{
	if (!vs)
		vs = new VertexShader();

	vs->Name = srcFile;

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		srcFile,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint,
		profile,
		shaderFlags,
		0,
		vs->Blob.ReleaseAndGetAddressOf(),
		errorBlob.ReleaseAndGetAddressOf()
	);

	if (FAILED(hr))  // Failed to compile
	{
		if (errorBlob)
		{
			LOG(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			COM_RELEASE(errorBlob);
		}
		HR(hr);  // If error halts here, check console for message
	}
	else  // Compiled warnings
	{
		if (errorBlob)
		{
			LOG(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			COM_RELEASE(errorBlob);
		}
	}
	COM_RELEASE(errorBlob);

	// Create vertex shader
	HR(m_device->CreateVertexShader(vs->Blob->GetBufferPointer(), vs->Blob->GetBufferSize(), nullptr, vs->Shader.ReleaseAndGetAddressOf()));


	// Create input layout from vertex shader
	// https://learn.microsoft.com/en-us/windows/win32/api/d3d11shader/nn-d3d11shader-id3d11shaderreflection
	{
		ComPtr<ID3D11ShaderReflection> vsReflection = nullptr;
		HR(D3DReflect(vs->Blob->GetBufferPointer(),
			vs->Blob->GetBufferSize(),
			IID_PPV_ARGS(vsReflection.ReleaseAndGetAddressOf()))  // Macro usage from https://learn.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-iid_ppv_args
		);

		CREATE_ZERO(D3D11_SHADER_DESC, desc);
		HR(vsReflection->GetDesc(&desc));

		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayout;
		for (UINT i = 0; i < desc.InputParameters; i++)
		{
			// Get input parameter at index
			CREATE_ZERO(D3D11_SIGNATURE_PARAMETER_DESC, paramDesc);
			vsReflection->GetInputParameterDesc(i, &paramDesc);

			// Create input element descripton
			CREATE_ZERO(D3D11_INPUT_ELEMENT_DESC, elementDesc);
			elementDesc.SemanticName = paramDesc.SemanticName;
			elementDesc.SemanticIndex = paramDesc.SemanticIndex;
			elementDesc.InputSlot = 0;
			elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			// Determine DXGI format
			if (paramDesc.Mask == 1)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (paramDesc.Mask <= 3)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (paramDesc.Mask <= 7)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (paramDesc.Mask <= 15)
			{
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
					elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}

			inputLayout.push_back(elementDesc);
		}

		HR(m_device->CreateInputLayout(
			&inputLayout[0],
			(UINT)inputLayout.size(),
			vs->Blob->GetBufferPointer(),
			vs->Blob->GetBufferSize(),
			vs->InputLayout.ReleaseAndGetAddressOf()
		));

		COM_RELEASE(vsReflection);
	}

#ifdef _DEBUG
	std::wstring wfile(srcFile);
	LOG("Created vertex shader from file: " << std::string(wfile.begin(), wfile.end()));
#endif // _DEBUG

}

void Direct3D::CreatePixelShader(PixelShader*& ps, LPCWSTR srcFile, LPCSTR profile, LPCSTR entryPoint)
{
	if (!ps)
		ps = new PixelShader();

	ps->Name = srcFile;

	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif // _DEBUG

	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3DCompileFromFile(
		srcFile,
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint,
		profile,
		shaderFlags,
		0,
		ps->Blob.ReleaseAndGetAddressOf(),
		errorBlob.ReleaseAndGetAddressOf()
	);

	if (FAILED(hr))  // Failed to compile
	{
		if (errorBlob)
		{
			// TODO: Log error message (handle hot reloading here)
			LOG(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			COM_RELEASE(errorBlob);
		}
		HR(hr);
	}
	else  // Compiled warnings
	{
		if (errorBlob)
		{
			LOG(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			COM_RELEASE(errorBlob);
		}
	}
	COM_RELEASE(errorBlob);

	HR(m_device->CreatePixelShader(ps->Blob->GetBufferPointer(), ps->Blob->GetBufferSize(), nullptr, ps->Shader.ReleaseAndGetAddressOf()));

#ifdef _DEBUG
	std::wstring wfile(srcFile);
	LOG("Created pixel shader from file: " << std::string(wfile.begin(), wfile.end()));
#endif // _DEBUG
}


void Direct3D::CreateConstantBuffer(ComPtr<ID3D11Buffer>& buf, UINT size, D3D11_USAGE usage, UINT cpuAccess)
{
	CREATE_ZERO(D3D11_BUFFER_DESC, cbd);
	cbd.Usage          = usage;
	cbd.ByteWidth      = size;
	cbd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = cpuAccess;
	HR(m_device->CreateBuffer(&cbd, nullptr, buf.ReleaseAndGetAddressOf()));
}
