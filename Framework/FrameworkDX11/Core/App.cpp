#include "pch.h"
#include "App.h"

#include "Core/Window.h"
#include "Core/Direct3D.h"
#include "Core/Shaders.h"
#include "Core/Buffers.h"
#include "Core/GameObject.h"
#include "Utils/Primitives.h"


#ifdef ENABLE_IMGUI
#include <Imgui/imgui_impl_dx11.h>
#include <Imgui/imgui_impl_win32.h>
#include "Imgui/imgui.h"  
#endif // ENABLE_IMGUI



App::App(HINSTANCE hInst)
	:
	m_window(nullptr),
	m_vertexShader(nullptr),
	m_pixelShader(nullptr),
	m_constantBuffer(nullptr),
	m_lightCBuffer(nullptr),
	m_gameObject(nullptr)
{
	m_window = new Window(hInst, 1280, 720);
}

App::~App()
{
	delete m_window;
	_CrtDumpMemoryLeaks();
}

bool App::Init()
{
	if (!D3D->Init(m_window->GetHandle(), true))
		return false;

	// Create the shaders
	D3D->CreateVertexShader(m_vertexShader, L"shader.fx");
	D3D->CreatePixelShader(m_pixelShader, L"shader.fx");

	// Create the constant buffers
	D3D->CreateConstantBuffer(m_constantBuffer, sizeof(VSConstantBuffer));
	D3D->CreateConstantBuffer(m_lightCBuffer,   sizeof(LightPropertiesConstantBuffer));
	
#ifdef ENABLE_IMGUI
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO();
	ImGui_ImplWin32_Init(m_window->GetHandle());
	ImGui_ImplDX11_Init(D3D_DEVICE, D3D_CONTEXT);
	ImGui::StyleColorsDark();
#endif // ENABLE_IMGUI

	// Create test gameobject
	namespace cube = Primitives::Cube;
	m_gameObject = new GameObject(cube::VerticesByteWidth, 
								  cube::Vertices,
								  cube::IndicesByteWidth,
							 	  cube::Indices);
	
	
	
	auto eye = sm::Vector4(0.0f, 0, -3, 1.0f);
	auto at = sm::Vector4(0.0f, 0.0f, 0.0f, 0.0f);
	auto up = sm::Vector4(0.0f, 1.0f, 0.0f, 0.0f);
	view = XMMatrixLookAtLH(eye, at, up);
	auto aspectRatio = (float)m_window->GetClientWidth() / (float)m_window->GetClientHeight();
	projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, aspectRatio, 0.01f, 100.0f);


	return true;
}

void App::Shutdown()
{
	delete m_constantBuffer;
	delete m_lightCBuffer;

	delete m_gameObject;
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
		D3D->BeginFrame({ 0.01f, 0.01f, 0.11f, 1.0f });  // Set scene background color

		OnUpdate(0.0);
		OnRender();
#ifdef ENABLE_IMGUI	
		OnGui(); 
#endif

		D3D->EndFrame();  // Present to screen
	}
}


void App::CalculateLighting()
{
	CREATE_ZERO(Light, light);
	light.Enabled              = static_cast<int>(TRUE);
	light.LightType            = PointLight;
	light.Color                = XMFLOAT4(Colors::White);
	light.SpotAngle            = XMConvertToRadians(45.0f);
	light.ConstantAttenuation  = 1.0f;
	light.LinearAttenuation    = 1;
	light.QuadraticAttenuation = 1;
	light.Position = sm::Vector4(0.0f, 0, -3, 1.0f);
	auto dir = sm::Vector4(light.Position.x, light.Position.y, light.Position.z, 0.0f);
	dir.Normalize();
	light.Direction = dir;

	CREATE_ZERO(LightPropertiesConstantBuffer, lightCb);
	lightCb.EyePosition = light.Position;
	lightCb.Lights[0] = light;

	D3D_CONTEXT->UpdateSubresource(m_lightCBuffer->Buffer.Get(), 0, nullptr, &lightCb, 0, 0);
}

void App::OnUpdate(double dt)
{
	float speed = 0.015f;

	if (GetAsyncKeyState(VK_SHIFT))
		m_gameObject->m_position.z -= speed;
	if (GetAsyncKeyState(VK_CONTROL))
		m_gameObject->m_position.z += speed;


	m_gameObject->Update(dt);
}

void App::OnRender()
{
	CREATE_ZERO(VSConstantBuffer, cb);
	cb.mWorld       = m_gameObject->m_world.Transpose();
	cb.mView        = view;
	cb.mProjection  = projection;
	cb.vOutputColor = XMFLOAT4(0, 0, 0, 0);
	D3D_CONTEXT->UpdateSubresource(m_constantBuffer->Buffer.Get(), 0, nullptr, &cb, 0, 0);
	

	D3D_CONTEXT->IASetInputLayout(m_vertexShader->InputLayout.Get());
	D3D_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D_CONTEXT->VSSetShader(m_vertexShader->Shader.Get(), nullptr, 0);
	D3D_CONTEXT->PSSetShader(m_pixelShader->Shader.Get(), nullptr, 0);
	D3D_CONTEXT->VSSetConstantBuffers(0, 1, m_constantBuffer->Buffer.GetAddressOf());
	D3D_CONTEXT->PSSetConstantBuffers(2, 1, m_lightCBuffer->Buffer.GetAddressOf());


	m_gameObject->Set();
	CalculateLighting();


	m_gameObject->Draw();
}

#ifdef ENABLE_IMGUI
void App::OnGui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// UI render here
	{
		ImGui::Begin("Imgui Test");
		ImGui::Text("Hello");
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
#endif // ENABLE_IMGUI
