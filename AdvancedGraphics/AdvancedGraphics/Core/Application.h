#pragma once
#include "Core/Timer.h"
#include "Core/Window.h"
#include "Graphics/Direct3D.h"
#include "Core/GameObject.h"
#include "Core/Camera.h"


#define ENABLE_IMGUI 1

#define DEFAULT_SHADER L"Shaders/Shader.fx"

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
	void CreateRenderTarget();

	void OnUpdate(double dt);
	void OnRender();

#if ENABLE_IMGUI
	void OnGui();
#endif // ENABLE_IMGUI

private:
	Timer                m_appTimer;
	Window*              m_window;

	VertexShader*        m_vertexShader;
	PixelShader*         m_pixelShader;

	ComPtr<ID3D11Buffer> m_constantBuffer;
	ComPtr<ID3D11Buffer> m_lightCBuffer;

	GameObject*          m_gameObject;
	GameObject*          m_goLight;
	Camera               m_camera;

	sm::Vector3          m_lightPosition;
	sm::Vector3          m_lightDiffuse;
	sm::Vector3          m_lightSpecular;
	sm::Vector3          m_lightAttenuation;
	sm::Vector4          m_parallaxData;
};