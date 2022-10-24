#include "pch.h"
#include "Application.h"
#include "Core/Core.h"
#include "Graphics/Primitives.h"


// Include imgui headers
#if ENABLE_IMGUI
#include <Imgui/imgui_impl_dx11.h>
#include <Imgui/imgui_impl_win32.h>
#include "Imgui/imgui.h"  
#endif // ENABLE_IMGUI



Application::Application(HINSTANCE hInst, UINT width, UINT height)
	:
	m_window(nullptr),
	m_vertexShader(nullptr),
	m_pixelShader(nullptr),
	m_gameObject(nullptr),
	m_goLight(nullptr),
	m_appTimer(Timer()),
	m_lightPosition(-2.0f, 1.5f, -2.0f),
	m_lightDiffuse(Colors::White),
	m_lightSpecular(Colors::White),
	m_lightAttenuation(1.0f)
{
	CREATE_AND_ATTACH_CONSOLE();

	m_window = new Window(hInst, width, height);
	m_camera = Camera(90.0f, (float)m_window->GetClientWidth(), (float)m_window->GetClientHeight());
	m_camera.Position(sm::Vector3(0.0f, 0.0f, -8.0f));
}

Application::~Application()
{
	COM_RELEASE(m_constantBuffer);

	SAFE_DELETE(m_goLight);
	SAFE_DELETE(m_gameObject)
	SAFE_DELETE(m_vertexShader);
	SAFE_DELETE(m_pixelShader);
	SAFE_DELETE(m_window);
}



bool Application::Init()
{
	if (!D3D->Init(m_window->GetHandle(), false))
	{
		LOG("Failed to initialize Direct3D");
		return false;
	}
	
#if ENABLE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO();
	ImGui_ImplWin32_Init(m_window->GetHandle());
	ImGui_ImplDX11_Init(D3D_DEVICE, D3D_CONTEXT);
	ImGui::StyleColorsDark();

#endif // ENABLE_IMGUI


	// Create shaders
	D3D->CreateVertexShader(m_vertexShader, DEFAULT_SHADER);
	D3D->CreatePixelShader(m_pixelShader, DEFAULT_SHADER);


	// Create and set game objects properties
	m_gameObject = new GameObject();
	m_gameObject->InitMesh("Assets\\SmoothCube.obj");
	m_gameObject->SetTexture(L"Assets\\MetalGrateDIFF.dds", L"Assets\\MetalGrateNORM.dds");
	
	// Visualizer for light position
	m_goLight = new GameObject();
	GO_CREATE_MESH(m_goLight, Primitives::Triangle);
	m_goLight->SetTexture(L"Assets\\stone.dds");
	m_goLight->m_scale = sm::Vector3(0.5f);


	D3D->CreateConstantBuffer(m_constantBuffer, sizeof(VSConstantBuffer));
	D3D->CreateConstantBuffer(m_lightCBuffer, sizeof(LightProperties));

	D3D_CONTEXT->VSSetShader(m_vertexShader->Shader.Get(), nullptr, 0);
	D3D_CONTEXT->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	D3D_CONTEXT->PSSetConstantBuffers(2, 1, m_lightCBuffer.GetAddressOf());
	D3D_CONTEXT->VSSetConstantBuffers(2, 1, m_lightCBuffer.GetAddressOf());

	D3D_CONTEXT->PSSetShader(m_pixelShader->Shader.Get(), nullptr, 0);
		
	D3D_CONTEXT->IASetInputLayout(m_vertexShader->InputLayout.Get());
	D3D_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D->SetWireframe(false);

	return true;
}

void Application::Run()
{
	m_appTimer.Reset();
	m_appTimer.Start();

	while (m_window->ProcessMessages())
	{
		m_appTimer.Tick();

		D3D->BeginFrame({ 0.01f, 0.01f, 0.01f, 1.0f });
		OnUpdate(m_appTimer);
		OnRender();
#if ENABLE_IMGUI
		OnGui();
#endif // ENABLE_IMGUI

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


	auto& pos = m_camera.Position();
	LightProperties lightProperties = LightProperties();
	lightProperties.EyePosition     = sm::Vector4(pos.x, pos.y, pos.z, 1.0f);
	lightProperties.PointLight      = light;

	// Updat lighting constant buffer
	D3D_CONTEXT->UpdateSubresource(m_lightCBuffer.Get(), 0, nullptr, &lightProperties, 0, 0);
}

void Application::OnUpdate(double dt)
{
	m_goLight->m_position = m_lightPosition;
	if (KEYBOARD.X)
		m_gameObject->m_rotation.x += (float)dt;
	if (KEYBOARD.Y)
		m_gameObject->m_rotation.y += (float)dt;
	if (KEYBOARD.Z)
		m_gameObject->m_rotation.z += (float)dt;

	if (KEYBOARD.G)
		m_gameObject->m_position.x += (float)dt;
	if (KEYBOARD.H)
		m_gameObject->m_position.x -= (float)dt;

	
	m_camera.Update(dt, KEYBOARD, MOUSE);

	m_gameObject->Update(dt);
	m_goLight->Update(dt);	
}

void Application::OnRender()
{
	CalculateLighting();

	CREATE_ZERO(VSConstantBuffer, cb);
	cb.View = m_camera.GetView().Transpose();
	cb.Projection = m_camera.GetProjection().Transpose();


	cb.World = m_goLight->GetWorldTransform().Transpose();
	D3D_CONTEXT->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &cb, 0, 0);
	
	D3D->SetCullMode(false);
	m_goLight->Draw();

	// Update constant bufffers
	cb.World = m_gameObject->GetWorldTransform().Transpose();
	D3D_CONTEXT->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &cb, 0, 0);

	D3D->SetCullMode(true);
	m_gameObject->Draw();
}


#if ENABLE_IMGUI
void Application::OnGui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// UI render here
	{
		ImGui::Begin("Editor");
		ImGui::SetWindowPos({ 0,0 }, ImGuiCond_Always);
		ImGui::SetWindowSize({ 350, static_cast<float>(m_window->GetClientHeight()) }, ImGuiCond_Once);

		if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("Light Position", &m_lightPosition.x, 0.1f, -50.0f, 50.0f);

			ImGui::Spacing();
			ImGui::ColorEdit3("Light Diffuse", &m_lightDiffuse.x, ImGuiColorEditFlags_Float);
			ImGui::ColorEdit3("Light Specular", &m_lightSpecular.x, ImGuiColorEditFlags_Float);
			ImGui::DragFloat3("Light Attenuation", &m_lightAttenuation.x, 0.001f,  0.00f,  100.0f);

			auto pos = m_camera.Position();
			ImGui::DragFloat3("Camera", &pos.x);
		}
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
#endif // ENABLE_IMGUI