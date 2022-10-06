#include "pch.h"
#include "App.h"
#include "Core/Window.h"
#include "Core/Direct3D.h"
#include "Core/Shaders.h"
#include <Imgui/imgui_impl_dx11.h>
#include <Imgui/imgui_impl_win32.h>

#include "Imgui/imgui.h"

App::App(HINSTANCE hInst)
	:
	m_window(nullptr)
{
	//m_window = std::make_unique<Window>(hInst, 1280, 720);
	m_window = new Window(hInst, 1280, 720);
	auto s = sizeof(m_window);
}

App::~App()
{
	delete m_window;
}

bool App::Init()
{
	if (!D3D->Init(m_window->GetHandle(), true))
		return false;

	m_vertexShader = new VertexShader();
	D3D->CreateVertexShader(m_vertexShader, L"shader.fx");

	m_pixelShader = new PixelShader();
	D3D->CreatePixelShader(m_pixelShader, L"shader.fx");


	// Create the constant buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage             = D3D11_USAGE_DEFAULT;
	bd.ByteWidth         = sizeof(SimpleVertex) * 24;
	bd.BindFlags         = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags    = 0;
	bd.Usage             = D3D11_USAGE_DEFAULT;
	bd.ByteWidth         = sizeof(WORD) * 36;        // 36 vertices needed for 12 triangles in a triangle list
	bd.BindFlags         = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags    = 0;
	bd.Usage             = D3D11_USAGE_DEFAULT;
	bd.ByteWidth         = sizeof(ConstantBuffer);
	bd.BindFlags         = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags    = 0;
	HR(D3D->GetDevice()->CreateBuffer(&bd, nullptr, m_constantBuffer.ReleaseAndGetAddressOf()));

	// Create the light constant buffer
	bd.Usage          = D3D11_USAGE_DEFAULT;
	bd.ByteWidth      = sizeof(LightPropertiesConstantBuffer);
	bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	HR(D3D->GetDevice()->CreateBuffer(&bd, nullptr, m_lightCBuffer.ReleaseAndGetAddressOf()));

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(m_window->GetHandle());
	ImGui_ImplDX11_Init(D3D->GetDevice(), D3D->GetContext());
	ImGui::StyleColorsDark();
	
	D3D->GetContext()->IASetInputLayout(m_vertexShader->InputLayout.Get());
	return true;
}

void App::Shutdown()
{
	// Release COM objects
	COM_RELEASE(m_constantBuffer);
	COM_RELEASE(m_lightCBuffer);

	delete m_vertexShader;
	delete m_pixelShader;

	D3D->Shutdown();
	Direct3D::Kill();
}

void App::Run()
{
	//auto a = D3D->CreateVertexShader(L"#$");
	while (m_window->ProcessMessages())
	{
		// Set scene background color
		D3D->BeginFrame({ 0.01f, 0.01f, 0.01f, 1.0f });


		// TODO: Make vertex and pixel shaders, mayber use classs instead of structs
		// Make shaders hot reloadable 
		// Maybe generate input layout at runtime



		OnUpdate(0.0);
		OnRender(0.0);
		OnGui(0.0);

		// Present to screen
		D3D->EndFrame();
	}
}



void App::OnUpdate(double dt)
{
	dt;
}

void App::OnRender(double dt)
{
	dt;
}

void App::OnGui(const double dt)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// UI render here
	{
		ImGui::ShowDemoWindow();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}