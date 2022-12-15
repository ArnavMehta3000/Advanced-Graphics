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

/*
* ----- References -----
* Aarthificial: https://www.youtube.com/watch?v=R6vQ9VmMz2w&t=344s
* Tile based rendering: https://www.digipen.edu/sites/default/files/public/docs/theses/denis-ishmukhametov-master-of-science-in-computer-science-thesis-efficient-tile-based-deferred-shading-pipeline.pdf
* 3DGEP: https://www.3dgep.com/forward-plus/#Deferred_Shading
* Learn OpenGL: https://learnopengl.com/Advanced-Lighting/Deferred-Shading
* Deferred rendering with light volumes: https://community.khronos.org/t/deferred-shading-and-light-volumes/69323
* 
*/





Application::Application(HINSTANCE hInst, UINT width, UINT height)
	:
	m_window(nullptr),
	m_appTimer(Timer()),
	m_lightPosition(-2.0f, 1.5f, -2.0f),
	m_globalAmbient(0.1f, 0.1f, 0.1f, 1.0f),
	m_lightRadius(5.0f),
	m_lightIntensity(1.0f),
	m_lightDiffuse(Colors::White),
	m_lightSpecular(Colors::LightGreen),
	m_parallaxData(8.0f, 32.0f, 0.05f, 0.05f),
	m_biasData(0.01f, 0.01f, 0.0f, 0.0f),
	m_technique(RenderTechnique::Forward),
	m_quadIB(nullptr),
	m_quadVB(nullptr),
	m_lightPropsCB(nullptr),
	m_offset(0),
	m_stride(0),
	m_quadIndicesCount(6),
	m_specularPower(10.0f),
	m_vigRadSoft(0.4f, 0.2f),
	m_enableVignette(true),
	m_enableGrayscale(false),
	m_showPreview(false),
	m_currentViewProj(sm::Matrix::Identity),
	m_prevViewProj(sm::Matrix::Identity)
{
	CREATE_AND_ATTACH_CONSOLE();
	LOG("----- DEBUG CONSOLE ATTACHED -----");

	m_window = new Window(hInst, width, height);
	m_camera = Camera(90.0f, (float)m_window->GetClientWidth(), (float)m_window->GetClientHeight(), 0.01f, 100.0f);
	m_camera.Position(sm::Vector3(0.0f, 0.0f, -8.0f));
}

Application::~Application()
{
	SAFE_DELETE(m_window);
}



bool Application::Init()
{
	if (!D3D->Init(m_window->GetHandle(), true, 4))
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
	SetTechnique(RenderTechnique::Forward);

	m_fsqShader      = Shader(L"Shaders\\FSQuad.hlsl", L"Shaders\\FSQuad.hlsl");
	m_forwardShader  = Shader(L"Shaders\\Shader.hlsl", L"Shaders\\Shader.hlsl");
	m_geometryShader = Shader(L"Shaders\\Advanced\\Geometry.hlsl", L"Shaders\\Advanced\\Geometry.hlsl");
	m_lightingShader = Shader(L"Shaders\\Advanced\\Lighting.hlsl", L"Shaders\\Advanced\\Lighting.hlsl");
	
	// Init game objects
	CREATE_ZERO(GODesc, cube);
	cube.MeshFile             = "Assets\\Cube.obj";
	cube.DiffuseTexture       = L"Assets\\rock_diffuse2.dds";
	cube.NormalMap            = L"Assets\\rock_bump.dds";
	cube.HeightMap            = L"Assets\\rock_height.dds";
	cube.PrimitiveType        = Primitives::Type::NONE;
	cube.IsPrimitive          = false;
	cube.HasMesh              = true;
	cube.HasDiffuse           = true;
	cube.HasNormal            = true;
	cube.HasHeight            = true;
	cube.IsEmmissive          = false;
	m_gameObjects.push_back(new GameObject(cube));
	m_gameObjects[0]->m_scale = sm::Vector3(2.0f);
		
	InitConstantBuffers();
	CreateQuad();

	D3D_CONTEXT->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D->SetWireframe(false);
	D3D->SetCullMode(true);


	LOG("----- APPLICATION INITIALIZATION FINISHED -----");
	
	return true;
}

void Application::CreateQuad()
{
	// Create fullscreen quad vertex buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, vbd);
	vbd.Usage              = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth          = Primitives::FSQuad::VerticesByteWidth;
	vbd.BindFlags          = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags     = 0;
	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, vertexInitData);
	vertexInitData.pSysMem = Primitives::FSQuad::Vertices;
	HR(D3D_DEVICE->CreateBuffer(&vbd, &vertexInitData, m_quadVB.ReleaseAndGetAddressOf()));

	m_stride           = Primitives::FSQuad::VerticesTypeSize;
	m_offset           = 0;
	m_quadIndicesCount = Primitives::FSQuad::IndicesCount;

	// Create fullscreen quad index buffer
	CREATE_ZERO(D3D11_BUFFER_DESC, ibd);
	ibd.Usage             = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth         = Primitives::FSQuad::IndicesByteWidth;
	ibd.BindFlags         = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags    = 0;
	CREATE_ZERO(D3D11_SUBRESOURCE_DATA, indexInitData);
	indexInitData.pSysMem = Primitives::FSQuad::Indices;
	HR(D3D_DEVICE->CreateBuffer(&ibd, &indexInitData, m_quadIB.ReleaseAndGetAddressOf()));
}

void Application::ClearAllCB()
{
	ID3D11Buffer* cb[] = {nullptr, nullptr, nullptr, nullptr};
	D3D_CONTEXT->VSSetConstantBuffers(0, _countof(cb), cb);
	D3D_CONTEXT->PSSetConstantBuffers(0, _countof(cb), cb);
}

void Application::InitGBuffer()
{
	RECT r;
	GetClientRect(m_window->GetHandle(), &r);
	UINT width = r.right - r.left;
	UINT height = r.bottom - r.top;

	m_lightTarget          = RenderTarget(DXGI_FORMAT_R32G32B32A32_FLOAT, width, height, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	m_colorTarget       = RenderTarget(DXGI_FORMAT_R32G32B32A32_FLOAT, width, height, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	m_normalTarget      = RenderTarget(DXGI_FORMAT_R11G11B10_FLOAT,    width, height, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	m_depthRenderTarget = RenderTarget(DXGI_FORMAT_R8G8B8A8_UNORM,     width, height, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

	LOG("Created G-Buffer");
}

void Application::InitConstantBuffers()
{
	D3D->CreateConstantBuffer(m_wvpCBuffer, sizeof(WVPBuffer));
	D3D->CreateConstantBuffer(m_cameraBuffer, sizeof(LightBuffer));
	D3D->CreateConstantBuffer(m_lightPropsCB, sizeof(LightProperties));
	D3D->CreateConstantBuffer(m_postProcessCB, sizeof(PostProcessing));
}


void Application::SetGBuffer()
{
	ID3D11RenderTargetView* rtv[] = { m_colorTarget.RTV().Get(), m_normalTarget.RTV().Get(), m_depthRenderTarget.RTV().Get() };

	// Clear the render targets
	for (auto& rt : rtv)
		D3D_CONTEXT->ClearRenderTargetView(rt, Colors::DarkKhaki);

	// Clear depth before geometry pass
	D3D_CONTEXT->ClearDepthStencilView(D3D->m_depthTarget.DSV().Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1u, 0u);

	D3D_CONTEXT->OMSetRenderTargets(_countof(rtv), rtv, D3D->m_depthTarget.DSV().Get());
}

void Application::DoGeometryPass()
{
	D3D_CONTEXT->OMSetDepthStencilState(D3D->m_depthWriteState.Get(), 1u);

	// Bind geometry pass shader
	m_geometryShader.BindShader();

	D3D_CONTEXT->PSSetSamplers(0, 1, D3D->m_samplerAnisotropicWrap.GetAddressOf());

	// Set wvp constant buffer
	D3D_CONTEXT->VSSetConstantBuffers(0, 1, m_wvpCBuffer.GetAddressOf());
	D3D_CONTEXT->PSSetConstantBuffers(0, 1, m_wvpCBuffer.GetAddressOf());

	// Update WVP constant buffer
	CREATE_ZERO(WVPBuffer, wvpCB);
	wvpCB.View       = m_camera.GetView().Transpose();
	wvpCB.Projection = m_camera.GetProjection().Transpose();

	CREATE_ZERO(SurfaceProperties, surfaceCB);
	surfaceCB = SurfaceProperties();  // Use default values

	for (auto& go : m_gameObjects)
	{
		wvpCB.World = go->GetWorldTransform().Transpose();
		D3D_CONTEXT->UpdateSubresource(m_wvpCBuffer.Get(), 0, nullptr, &wvpCB, 0, 0);

		// Update surface properties constant buffer
		D3D_CONTEXT->UpdateSubresource(go->m_surfacePropsCB.Get(), 0, nullptr, &surfaceCB, 0, 0);

		// Set object textures and depth target
		ID3D11ShaderResourceView* textureSrv[] = { go->GetDiffuseSRV().Get(), go->GetNormalSRV().Get(), go->GetHeightSRV().Get() };
		D3D_CONTEXT->PSSetShaderResources(0, _countof(textureSrv), textureSrv);

		go->Draw();
	}

	D3D_CONTEXT->ClearDepthStencilView(D3D->m_depthTarget.DSV().Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void Application::DoLightingPass()
{
	D3D_CONTEXT->OMSetDepthStencilState(D3D->m_depthReadState.Get(), 1u);

	// Set light accumulation target
	D3D_CONTEXT->OMSetRenderTargets(1, m_lightTarget.RTV().GetAddressOf(), nullptr);
	// Only one light in the scene hence no for-loop here

	// bind lighting pass shader
	m_lightingShader.BindShader();
	
	D3D_CONTEXT->PSSetSamplers(0, 1, D3D->m_samplerAnisotropicWrap.GetAddressOf());

	// Set constant buffers
	D3D_CONTEXT->VSSetConstantBuffers(0, 1, m_wvpCBuffer.GetAddressOf());
	ID3D11Buffer* psCB[] = { m_wvpCBuffer.Get(), m_cameraBuffer.Get()};
	D3D_CONTEXT->PSSetConstantBuffers(0, _countof(psCB), psCB);
	
	
	// Update light and camera constant buffer
	CREATE_ZERO(LightBuffer, camCBuffer);
	camCBuffer.GlobalAmbient            = m_globalAmbient;
	camCBuffer.EyePosition              = TO_VEC4(m_camera.Position(), 1.0f);
	camCBuffer.PointLight.Diffuse       = m_lightDiffuse;
	camCBuffer.PointLight.Position      = TO_VEC4(m_lightPosition, 1.0f);
	camCBuffer.PointLight.Specular      = m_lightSpecular;
	camCBuffer.PointLight.Intensity     = m_lightIntensity;
	camCBuffer.PointLight.SpecularPower = m_specularPower;
	camCBuffer.PointLight.Parallax      = m_parallaxData;
	camCBuffer.PointLight.Bias          = m_biasData;
	camCBuffer.PointLight.Radius        = m_lightRadius;

	D3D_CONTEXT->UpdateSubresource(m_cameraBuffer.Get(), 0, nullptr, &camCBuffer, 0, 0);
	
	// bind render targets as srv
	ID3D11ShaderResourceView* srv[]{ m_colorTarget.SRV().Get(), m_normalTarget.SRV().Get(), m_depthRenderTarget.SRV().Get() };
	D3D_CONTEXT->PSSetShaderResources(0, _countof(srv), srv);

	DrawQuad();
}


void Application::Run()
{
	m_appTimer.Reset();
	m_appTimer.Start();

	while (m_window->ProcessMessages())
	{
		m_appTimer.Tick();
		
		ClearAllCB();
		UpdateWorld(m_appTimer);

		switch (m_technique)
		{
		case RenderTechnique::Forward:
			DoForwardRendering();
			break;
		case RenderTechnique::Deferred:
			DoDeferredRendering();
			break;
		}

		// Expected Back buffer to be bound at this stage
		
		OnGui(m_appTimer);
		D3D->EndFrame();
	}
}


void Application::UpdateWorld(double dt)
{
	m_camera.Update(dt, KEYBOARD, MOUSE);
	m_prevViewProj    = m_currentViewProj;
	m_currentViewProj = m_camera.GetViewProjection();

	for (auto& go : m_gameObjects)
	{
		go->Update(dt);
	}
}

void Application::DoDeferredRendering()
{
	SetGBuffer();
	DoGeometryPass();
	D3D->UnbindAllTargetsAndResources();
	DoLightingPass();
	D3D->BindBackBuffer();
	DoPostProcess();	
}

void Application::DoPostProcess()
{
	m_fsqShader.BindShader();
	D3D_CONTEXT->PSSetConstantBuffers(0, 1, m_postProcessCB.GetAddressOf());

	// Update post processing
	PostProcessing pp{};
	pp.CurrentViewProjection  = m_currentViewProj.Transpose();
	pp.PrevViewProjection     = m_prevViewProj.Transpose();
	pp.VignetteRadiusSoftness = m_vigRadSoft;
	pp.EnableGrayscale        = m_enableGrayscale;
	pp.EnableVignette         = m_enableVignette;
	D3D_CONTEXT->UpdateSubresource(m_postProcessCB.Get(), 0, nullptr, &pp, 0, 0);

	// Set render texture
	D3D_CONTEXT->PSSetShaderResources(0, 1, m_lightTarget.SRV().GetAddressOf());
	D3D_CONTEXT->PSSetSamplers(0, 1, D3D->m_samplerAnisotropicWrap.GetAddressOf());

	DrawQuad();
}

void Application::DoForwardRendering()
{
	D3D->BindBackBuffer(false);

	D3D_CONTEXT->OMSetDepthStencilState(D3D->m_depthStateDefault.Get(), 1u);
	m_forwardShader.BindShader();
	D3D_CONTEXT->PSSetSamplers(0, 1, D3D->m_samplerAnisotropicWrap.GetAddressOf());

	// Set wvp constant buffer
	D3D_CONTEXT->VSSetConstantBuffers(0, 1, m_wvpCBuffer.GetAddressOf());
	D3D_CONTEXT->PSSetConstantBuffers(0, 1, m_wvpCBuffer.GetAddressOf());
	D3D_CONTEXT->PSSetConstantBuffers(2, 1, m_lightPropsCB.GetAddressOf());
	D3D_CONTEXT->VSSetConstantBuffers(2, 1, m_lightPropsCB.GetAddressOf());

	// Update lighting
	LightProperties lightProps{};
	lightProps.EyePosition            = TO_VEC4(m_camera.Position(), 1.0f);
	lightProps.GlobalAmbient          = m_globalAmbient;
	lightProps.PointLight.Position    = TO_VEC4(m_lightPosition, 1.0f);
	lightProps.PointLight.Diffuse     = TO_VEC4(m_lightDiffuse, 1.0f);
	lightProps.PointLight.Specular    = TO_VEC4(m_lightSpecular, 1.0f);
	lightProps.PointLight.Attenuation = sm::Vector4(1.0f, 0.7f, 1.8f, 0.0f);
	lightProps.PointLight.Parallax    = m_parallaxData;
	lightProps.PointLight.Bias        = m_biasData;
	D3D_CONTEXT->UpdateSubresource(m_lightPropsCB.Get(), 0, nullptr, &lightProps, 0, 0);
	

	// Update WVP constant buffer
	CREATE_ZERO(WVPBuffer, wvpCB);
	wvpCB.View       = m_camera.GetView().Transpose();
	wvpCB.Projection = m_camera.GetProjection().Transpose();

	for (auto& go : m_gameObjects)
	{
		wvpCB.World = go->GetWorldTransform().Transpose();
		D3D_CONTEXT->UpdateSubresource(m_wvpCBuffer.Get(), 0, nullptr, &wvpCB, 0, 0);

		// Update material buffers
		D3D_CONTEXT->UpdateSubresource(go->m_materialPropsCB.Get(), 0, nullptr, &go->m_material, 0, 0);
		D3D_CONTEXT->PSSetConstantBuffers(1, 1, go->m_materialPropsCB.GetAddressOf());

		ID3D11ShaderResourceView* textureSrv[] = { go->GetDiffuseSRV().Get(), go->GetNormalSRV().Get(), go->GetHeightSRV().Get() };
		D3D_CONTEXT->PSSetShaderResources(0, _countof(textureSrv), textureSrv);

		go->Draw();
	}
}

void Application::Shutdown()
{
	COM_RELEASE(m_wvpCBuffer);
	COM_RELEASE(m_cameraBuffer);
	COM_RELEASE(m_lightPropsCB);
	COM_RELEASE(m_quadIB);
	COM_RELEASE(m_quadVB);
	
	D3D->Shutdown();
	Direct3D::Kill();
}



void Application::OnGui(double dt)
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


		if (ImGui::Button("Forward"))
			SetTechnique(RenderTechnique::Forward);
		ImGui::SameLine();
		if (ImGui::Button("Deferred"))
			SetTechnique(RenderTechnique::Deferred);

		auto pos = m_camera.Position();
		ImGui::Text("Camera position: %.2f, %.2f, %.2f", pos.x, pos.y, pos.z);
		AddSpace(2);

		// ----- Info -----
		ImGui::Text("Current Rendering Technique: %s", (m_technique == RenderTechnique::Forward) ? "Forward" : "Deferred");
		ImGui::Text("FPS: %f", 1.0f / dt);
		ImGui::Text("Frame time: %f ms", dt * 1000.0f);



		// ----- WORLD SETTINGS -----
		if (ImGui::CollapsingHeader("World", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::DragFloat3("Cube Position", &m_gameObjects[0]->m_position.x, 0.1f, -100.0f, 100.0f, "%0.2f");
			ImGui::DragFloat3("Cube Rotation", &m_gameObjects[0]->m_rotation.x, 0.01f, -100.0f, 100.0f, "%0.2f");
			ImGui::DragFloat3("Cube Scale", &m_gameObjects[0]->m_scale.x, 0.01f, 0.01f, 100.0f, "%0.2f");
			AddSpace(2);

			ImGui::DragFloat3("Light Position", &m_lightPosition.x, 0.1f, -50.0f, 50.0f);
		}
		AddSpace();

		// ----- POINT LIGHT SETTINGS -----
		if (ImGui::CollapsingHeader("Point Light", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::ColorEdit3("Global Ambient", &m_globalAmbient.x, ImGuiColorEditFlags_Float);
			ImGui::ColorEdit3("Light Diffuse", &m_lightDiffuse.x, ImGuiColorEditFlags_Float);
			ImGui::ColorEdit3("Light Specular", &m_lightSpecular.x, ImGuiColorEditFlags_Float);
			if (m_technique == RenderTechnique::Deferred)
			{
				ImGui::DragFloat("Specular Power", &m_specularPower, 0.1f, 0.0f, 50.0f);
				ImGui::DragFloat("Light Intensity", &m_lightIntensity, 0.1f, 0.0f, 50.0f);
				ImGui::DragFloat("Light Radius", &m_lightRadius, 0.1f, 0.0f, 50.0f);
			}
		}
		AddSpace();

		// ----- PARALLAX MAPPING SETTINGS -----
		if (m_technique == RenderTechnique::Forward)
		{
			if (ImGui::CollapsingHeader("Parallax Mapping"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				ImGui::DragFloat("Min Layers", &m_parallaxData.x, 1, 100.0f, 5000.0f);
				ImGui::DragFloat("Max Layers", &m_parallaxData.y, 1, 500.0f, 5000.0f);
				ImGui::DragFloat("Height Scale", &m_parallaxData.z, 0.01f, -5.0f, 5.0f);
				ImGui::DragFloat("Shadow Factor", &m_parallaxData.w, 0.01f, 0.0f, 1.0f);
				ImGui::Spacing();
				ImGui::DragFloat2("Parallax Bias", &m_biasData.x, 0.001f, -1.0f, 1.0f);
			}
		}

		// ----- POST PROCESSING SETTINGS -----
		if (m_technique == RenderTechnique::Deferred)
		{
			ImGui::Spacing();
			if (ImGui::CollapsingHeader("Post Processing"), ImGuiTreeNodeFlags_DefaultOpen)
			{
				if (ImGui::TreeNode("Vignette"))
				{
					ImGui::Checkbox("Enable Vignette", &m_enableVignette);
					if (m_enableVignette)
					{
						ImGui::DragFloat("Radius", &m_vigRadSoft.x, 0.05f, 0.0f, 1.0f);
						ImGui::DragFloat("Softness", &m_vigRadSoft.y, 0.05f, -10.0f, 10.0f);
					}
					ImGui::TreePop();
				}

				if (ImGui::TreeNode("Grayscale"))
				{
					ImGui::Checkbox("Enable Grayscale", &m_enableGrayscale);
					ImGui::TreePop();
				}
				
				AddSpace(3);
				ImGui::Checkbox("Show G-Buffer", &m_showPreview);
				if (m_showPreview)
				{
					ImGui::Spacing();
					ImGui::SliderFloat("Preview Scale", &m_imageScale, 0.5f, 3.0f);
					ImVec2 imageSize = ImVec2(imguiWidth * m_imageScale, 197 * m_imageScale);

					ImGui::Text("Scene Diffuse");
					ImGui::Image((void*)m_colorTarget.SRV().Get(), imageSize);

					ImGui::Text("Scene Normals");
					ImGui::Image((void*)m_normalTarget.SRV().Get(), imageSize);

					ImGui::Text("Scene Pos-Depth (RGB-A)");
					ImGui::Image((void*)m_depthRenderTarget.SRV().Get(), imageSize);

					ImGui::Text("PP");
					ImGui::Image((void*)m_lightTarget.SRV().Get(), imageSize);
				}
			}
		}
		ImGui::End();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif // ENABLE_IMGUI
}

void Application::DrawQuad()
{
	// Draw full screen quad
	D3D_CONTEXT->IASetVertexBuffers(0, 1, m_quadVB.GetAddressOf(), &m_stride, &m_offset);
	D3D_CONTEXT->IASetIndexBuffer(m_quadIB.Get(), DXGI_FORMAT_R16_UINT, 0);
	D3D_CONTEXT->DrawIndexed(m_quadIndicesCount, 0, 0);
}

void Application::AddSpace(UINT n)
{
	for (UINT i = 0; i < n; i++)ImGui::Spacing();
}