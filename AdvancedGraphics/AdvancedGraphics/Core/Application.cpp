#include "pch.h"
#include "Application.h"
#include "Core/Core.h"
#include "Graphics/Primitives.h"


#define ENABLE_IMGUI 1

// Include imgui headers
#ifdef ENABLE_IMGUI
#include <Imgui/imgui_impl_dx11.h>
#include <Imgui/imgui_impl_win32.h>
#include "Imgui/imgui.h"    
#endif // ENABLE_IMGUI




Application::Application(HINSTANCE hInst, UINT width, UINT height)
	:
	m_window(nullptr),
	m_appTimer(Timer()),
	m_lightPosition(-2.0f, 1.5f, -2.0f),
	m_lightDiffuse(Colors::White),
	m_lightSpecular(Colors::White),
	m_lightAttenuation(1.0f),
	m_parallaxData(8.0f, 32.0f, 0.05f, 1.0f),
	m_biasData(0.01f, 0.01f, 0.0f, 0.0f)
{
	CREATE_AND_ATTACH_CONSOLE();
	LOG("----- DEBUG CONSOLE ATTACHED -----");

	m_window = new Window(hInst, width, height);
	m_camera = Camera(90.0f, (float)m_window->GetClientWidth(), (float)m_window->GetClientHeight());
	m_camera.Position(sm::Vector3(0.0f, 0.0f, -8.0f));
}

Application::~Application()
{
	SAFE_DELETE(m_window);
}



bool Application::Init()
{
	if (!D3D->Init(m_window->GetHandle(), false, 4))
	{
		LOG("Failed to initialize Direct3D");
		return false;
	}
	
	// Set up imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO();
	ImGui_ImplWin32_Init(m_window->GetHandle());
	ImGui_ImplDX11_Init(D3D_DEVICE, D3D_CONTEXT);
	ImGui::StyleColorsDark();

	InitGBuffer();
	
	D3D_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D->SetWireframe(false);
	D3D->SetCullMode(true);


	LOG("----- APPLICATION INITIALIZATION FINISHED -----");
	
	return true;
}

void Application::InitGBuffer()
{
	RECT r;
	GetClientRect(m_window->GetHandle(), &r);
	UINT width = r.right - r.left;
	UINT height = r.bottom - r.top;

	m_colorTarget    = RenderTarget(D3D->GetBackBufferFormat(), width, height, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	m_normalTarget   = RenderTarget(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	m_positionTarget = RenderTarget(DXGI_FORMAT_R16G16B16A16_UNORM, width, height, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	LOG("Created G-Buffer");
}

void Application::SetGBuffer()
{
	ID3D11RenderTargetView* rtv[] = { m_colorTarget.RTV().Get(), m_normalTarget.RTV().Get(), m_positionTarget.RTV().Get() };

	// Clear the render targets
	for (auto& rt : rtv)
		D3D_CONTEXT->ClearRenderTargetView(rt, Colors::BlanchedAlmond);

	// Clear depth before geometry pass
	D3D_CONTEXT->ClearDepthStencilView(D3D->m_depthTarget.DSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);

	D3D_CONTEXT->OMSetRenderTargets(_countof(rtv), rtv, D3D->m_depthTarget.DSV().Get());
}

void Application::DoGeometryPass()
{
}

void Application::DoLightingPass()
{
}

void Application::Run()
{
	m_appTimer.Reset();
	m_appTimer.Start();

	while (m_window->ProcessMessages())
	{
		m_appTimer.Tick();

		SetGBuffer();
		DoGeometryPass();
		D3D->UnbindAllTargetsAndResources();
		D3D->BindBackBuffer();
		DoLightingPass();

		OnGui();
		D3D->EndFrame();
	}
}

void Application::Shutdown()
{
	D3D->Shutdown();
	Direct3D::Kill();
}



void Application::OnGui()
{
	// Commented line 298 in inmgui_impl_dx11.cpp to stop the PSSETSHADERRESOURCE_HAZARD warnings

#ifdef ENABLE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// UI render here
	const float imguiWidth = 350.0f;
	const float wind32AspectRatio = static_cast<float>(m_window->GetClientWidth()) / static_cast<float>(m_window->GetClientHeight());
	{
		ImGui::Begin("Editor");
		ImGui::SetWindowPos({ 0,0 }, ImGuiCond_Always);
		ImGui::SetWindowSize({ imguiWidth, static_cast<float>(m_window->GetClientHeight()) }, ImGuiCond_Once);

		if (ImGui::CollapsingHeader("World", ImGuiTreeNodeFlags_DefaultOpen))
		{
			//ImGui::DragFloat3("Object Position", &m_gameObjects[0]->m_position.x, 0.01f, -100.0f, 100.0f, "%0.2f");
		}
		ImGui::Spacing();
		if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("Light Position", &m_lightPosition.x, 0.1f, -50.0f, 50.0f);
			ImGui::ColorEdit3("Light Diffuse", &m_lightDiffuse.x, ImGuiColorEditFlags_Float);
			ImGui::ColorEdit3("Light Specular", &m_lightSpecular.x, ImGuiColorEditFlags_Float);
			ImGui::DragFloat3("Light Attenuation", &m_lightAttenuation.x, 0.001f, 0.00f, 100.0f);

		}
		ImGui::Spacing();
		if (ImGui::CollapsingHeader("Parallax Mapping"))
		{
			ImGui::DragFloat("Min Layers", &m_parallaxData.x, 1, 5.0f, 50.0f);
			ImGui::DragFloat("Max Layers", &m_parallaxData.y, 1, 20.0f, 150.0f);
			ImGui::DragFloat("Height Scale", &m_parallaxData.z, 0.01f, -5.0f, 5.0f);
			ImGui::DragFloat("Shadow Factor", &m_parallaxData.w, 0.01f, 1.0f, 10.0f);
			ImGui::Spacing();
			ImGui::DragFloat2("Parallax Bias", &m_biasData.x, 0.001f, -1.0f, 1.0f);
		}
		ImGui::Spacing();
		if (ImGui::CollapsingHeader("Post Processing"))
		{
			ImGui::SliderFloat("Preview Scale", &m_imageScale, 0.5f, 3.0f);
			ImVec2 imageSize = ImVec2(imguiWidth * m_imageScale, 197 * m_imageScale);

			ImGui::Text("Scene Diffuse");
			ImGui::Image((void*)m_colorTarget.SRV().Get(), imageSize);
			
			ImGui::Text("Scene Normals");
			ImGui::Image((void*)m_normalTarget.SRV().Get(), imageSize);

			ImGui::Text("Scene World Position");
			ImGui::Image((void*)m_positionTarget.SRV().Get(), imageSize);

			ImGui::Text("Scene Depth");
			ImGui::Image((void*)D3D->m_depthTarget.SRV().Get(), imageSize);
		}
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif // ENABLE_IMGUI

}
