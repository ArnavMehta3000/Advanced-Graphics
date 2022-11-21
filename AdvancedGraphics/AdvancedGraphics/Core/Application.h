#pragma once
#include "Core/Timer.h"
#include "Core/Window.h"
#include "Graphics/Direct3D.h"
#include "Core/GameObject.h"
#include "Core/Camera.h"
#include "Graphics/RenderTexture.h"

#define DEFAULT_SHADER L"Shaders/Shader.hlsl"
#define TO_VEC4(vec, val) sm::Vector4(vec.x, vec.y, vec.z, val)

class Application
{
#define KEYBOARD m_window->GetKbState()
#define MOUSE m_window->GetMouseState()

public:
	Application(HINSTANCE hInst, UINT width, UINT height);
	~Application();

	bool Init();
	void Run();
	void Shutdown();

	
private:
	void CalculateLighting();

	void OnUpdate(double dt);
	void OnRender();
	void OnGui();

private:
	Timer                      m_appTimer;
	Window*                    m_window;

	RenderTarget*              m_renderTarget;
	std::vector<RenderTarget*> m_gBuffer;

	VertexShader*              m_vertexShader;
	PixelShader*               m_pixelShader;

	ComPtr<ID3D11Buffer>       m_constantBuffer;
	ComPtr<ID3D11Buffer>       m_lightCBuffer;

	std::vector<GameObject*>   m_gameObjects;
	Camera                     m_camera;

	float                      m_imageScale = 1.0f;

	sm::Vector3                m_ambientLight;
	sm::Vector3                m_lightPosition;
	sm::Vector3                m_lightDiffuse;
	sm::Vector3                m_lightSpecular;
	sm::Vector3                m_lightAttenuation;
	sm::Vector4                m_parallaxData;
	sm::Vector4                m_biasData;
};