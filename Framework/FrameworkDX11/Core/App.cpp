#include "pch.h"
#include "App.h"
#include "Core/Window.h"
#include "Core/Direct3D.h"
#include "Core/Shaders.h"

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

	// Delete application pointers
	delete m_vertexShader;
	delete m_pixelShader;
}

bool App::Init()
{
	if (!D3D->Init(m_window->GetHandle(), true))
		return false;

	m_vertexShader = new VertexShader();
	D3D->CreateVertexShader(m_vertexShader, L"shader.fx");

	m_pixelShader = new PixelShader();
	D3D->CreatePixelShader(m_pixelShader, L"shader.fx");


	/*IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(m_window->GetHandle());
	ImGui_ImplDX11_Init(D3D->GetDevice(), D3D->GetContext());
	ImGui::StyleColorsDark();*/
	
	D3D->GetContext()->IASetInputLayout(m_vertexShader->InputLayout.Get());
	return true;
}

void App::Shutdown()
{
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

}