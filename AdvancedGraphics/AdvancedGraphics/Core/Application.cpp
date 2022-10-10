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
	m_appTimer(Timer())
{
#ifdef _DEBUG
	CREATE_AND_ATTACH_CONSOLE();
#endif // _DEBUG

	m_window = new Window(hInst, width, height);
	m_camera = Camera(90.0f, (float)m_window->GetClientWidth(), (float)m_window->GetClientHeight());
}

Application::~Application()
{
	COM_RELEASE(m_constantBuffer);

	//SAFE_DELETE(m_camera);
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
	GO_CREATE_MESH(m_gameObject, Primitives::Cube);
	m_gameObject->Set();

	D3D->CreateConstantBuffer(m_constantBuffer, sizeof(VSConstantBuffer));
	D3D->CreateConstantBuffer(m_lightCBuffer, sizeof(LightProperties));

	D3D_CONTEXT->VSSetShader(m_vertexShader->Shader.Get(), nullptr, 0);
	D3D_CONTEXT->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());
	D3D_CONTEXT->PSSetConstantBuffers(2, 1, m_lightCBuffer.GetAddressOf());
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
	Light light;
	light.Enabled              = TRUE;
	light.LightType            = PointLight;
	light.Color                = XMFLOAT4(Colors::White);
	light.SpotAngle            = XMConvertToRadians(45.0f);
	light.ConstantAttenuation  = m_atten;
	light.LinearAttenuation    = 1;
	light.QuadraticAttenuation = 1;



	// set up the light
	light.Position = sm::Vector4(0.0f, 0.0f, -3.0f, 1.0f);
	light.Direction = light.Position * -1.0f;
	light.Direction.Normalize();

	LightProperties lightProperties;
	lightProperties.EyePosition = light.Position;
	lightProperties.Lights[0] = light;
	D3D_CONTEXT->UpdateSubresource(m_lightCBuffer.Get(), 0, nullptr, &lightProperties, 0, 0);
}

void Application::OnUpdate(double dt)
{
	m_camera.Update(dt, KEYBOARD, MOUSE);

	float speed = 1.0f;


	// Update constant bufffers
	CREATE_ZERO(VSConstantBuffer, cb);
	cb.World       = sm::Matrix::Identity;//::CreateTranslation(0,0,sin(m_appTimer.TotalTime())).Transpose();
	cb.View        = m_camera.GetView().Transpose();
	cb.Projection  = m_camera.GetProjection().Transpose();

	cb.OutputColor = { 0, 1, 0, 1 };
	D3D_CONTEXT->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &cb, 0, 0);

	
	// Update game objects
	m_gameObject->Update(dt);
}

void Application::OnRender()
{
	CalculateLighting();

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
		ImGui::Begin("Imgui Test");
		ImGui::SliderFloat("Attenuation", &m_atten, 0.5f, 10.0f);
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
#endif // ENABLE_IMGUI