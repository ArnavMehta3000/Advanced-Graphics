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
	m_vertexShader(nullptr),
	m_pixelShader(nullptr),
	m_renderTarget(nullptr),
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
	COM_RELEASE(m_constantBuffer);

	for (auto& go : m_gameObjects)
		SAFE_DELETE(go);

	SAFE_DELETE(m_vertexShader);
	SAFE_DELETE(m_pixelShader);

	SAFE_DELETE(m_window);

	SAFE_DELETE(m_renderTarget);
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

	// Create shaders
	D3D->CreateVertexShader(m_vertexShader, L"Shaders/Deffered.hlsl");
	D3D->CreatePixelShader(m_pixelShader, L"Shaders/Deffered.hlsl");


	CREATE_ZERO(GODesc, desc);
	desc.MeshFile             = "Assets\\Plane.obj";
	desc.DiffuseTexture       = L"Assets\\rock_diffuse2.dds";
	desc.NormalMap            = L"Assets\\rock_bump.dds";
	desc.HeightMap            = L"Assets\\rock_height.dds";
	desc.PrimitiveType        = Primitives::Type::NONE;
	desc.IsPrimitive          = false;
	desc.HasMesh              = true;
	desc.HasDiffuse           = true;
	desc.HasNormal            = true;
	desc.HasHeight            = true;
	desc.IsEmmissive          = false;
	m_gameObjects.push_back(new GameObject(desc));
	m_gameObjects[0]->m_scale = sm::Vector3(2.0f);



	// Visualizer for light position
	desc.MeshFile             = "Assets\\Sphere.obj";
	desc.DiffuseTexture       = L"Assets\\stone.dds";
	desc.HasDiffuse           = true;
	desc.HasNormal            = false;
	desc.HasHeight            = false;
	desc.IsEmmissive          = true;
	desc.EmmissiveColor       = sm::Vector3(10, 10, 10);
	m_gameObjects.push_back(new GameObject(desc));
	m_gameObjects[1]->m_scale = sm::Vector3(0.25f);

	// Create FS render target
	m_renderTarget = new RenderTarget(m_window->GetClientWidth(), m_window->GetClientHeight());


	// Create constant buffers
	D3D->CreateConstantBuffer(m_constantBuffer, sizeof(VSConstantBuffer));
	D3D->CreateConstantBuffer(m_lightCBuffer, sizeof(LightProperties));

	
	D3D_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D->SetWireframe(false);
	D3D->SetCullMode(true);


	LOG("----- APPLICATION INITIALIZATION FINISHED -----");
	
	return true;
}

void Application::Run()
{
	m_appTimer.Reset();
	m_appTimer.Start();

	while (m_window->ProcessMessages())
	{
		m_appTimer.Tick();
		SetWindowTextA(m_window->GetHandle(), ("FPS: " + std::to_string(1.0f / m_appTimer)).c_str());
		OnUpdate(m_appTimer);
		
		D3D->BindGBuffer();
		//D3D->BindRenderTarget(m_renderTarget);
		OnRender();
		D3D->DoLightingPass(m_renderTarget);
		D3D->UnbindAllRenderTargets();
		D3D->DrawFSQuad(m_renderTarget);
		D3D->BindBackBuffer();

		OnGui();
		D3D->EndFrame();
	}
}

void Application::Shutdown()
{
	D3D->Shutdown();
	Direct3D::Kill();
}



void Application::CalculateLighting()
{
	// Set up the light
	PointLight light  = PointLight();
	light.Position    = TO_VEC4(m_lightPosition, 1.0f);
	light.Diffuse     = TO_VEC4(m_lightDiffuse, 1.0f);
	light.Specular    = TO_VEC4(m_lightSpecular, 1.0f);
	light.Attenuation = sm::Vector4(m_lightAttenuation);
	light.Parallax    = m_parallaxData;
	light.Bias        = m_biasData;


	auto& pos = m_camera.Position();
	LightProperties lightProperties = LightProperties();
	lightProperties.EyePosition     = sm::Vector4(pos.x, pos.y, pos.z, 1.0f);
	lightProperties.PointLight      = light;

	// Updat lighting constant buffer
	D3D_CONTEXT->UpdateSubresource(m_lightCBuffer.Get(), 0, nullptr, &lightProperties, 0, 0);
}

void Application::OnUpdate(double dt)
{
	m_gameObjects[1]->m_position = m_lightPosition;

	if (KEYBOARD.X)
		m_gameObjects[0]->m_rotation.x += (float)dt;
	if (KEYBOARD.Y)
		m_gameObjects[0]->m_rotation.y += (float)dt;
	if (KEYBOARD.Z)
		m_gameObjects[0]->m_rotation.z += (float)dt;

	if (KEYBOARD.G)
		m_gameObjects[0]->m_position.x += (float)dt;
	if (KEYBOARD.H)
		m_gameObjects[0]->m_position.x -= (float)dt;

	
	m_camera.Update(dt, KEYBOARD, MOUSE);

	for (auto& go : m_gameObjects)
		go->Update(m_appTimer);
}

void Application::OnRender()
{
	// Set shaders for the objects
	D3D_CONTEXT->VSSetShader(m_vertexShader->Shader.Get(), nullptr, 0);
	D3D_CONTEXT->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	D3D_CONTEXT->PSSetConstantBuffers(2, 1, m_lightCBuffer.GetAddressOf());
	D3D_CONTEXT->VSSetConstantBuffers(2, 1, m_lightCBuffer.GetAddressOf());

	D3D_CONTEXT->PSSetShader(m_pixelShader->Shader.Get(), nullptr, 0);

	D3D_CONTEXT->IASetInputLayout(m_vertexShader->InputLayout.Get());

	CalculateLighting();


	CREATE_ZERO(VSConstantBuffer, cb);
	cb.View       = m_camera.GetView().Transpose();
	cb.Projection = m_camera.GetProjection().Transpose();

	for (auto& go : m_gameObjects)
	{
		cb.World = go->GetWorldTransform().Transpose();
		D3D_CONTEXT->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
		go->Draw();
	}
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
			ImGui::DragFloat3("Object Position", &m_gameObjects[0]->m_position.x, 0.01f, -100.0f, 100.0f, "%0.2f");
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
			ImGui::Image((void*)D3D->m_srvArray[0].Get(), imageSize);
			
			ImGui::Text("Scene Normals");
			ImGui::Image((void*)D3D->m_srvArray[1].Get(), imageSize);
			
			ImGui::Text("Scene World Position");
			ImGui::Image((void*)D3D->m_srvArray[2].Get(), imageSize);

			ImGui::Text("Raw Scene");
			ImGui::Image((void*)D3D->m_depthStencilView.Get(), imageSize);
		}
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif // ENABLE_IMGUI

}
