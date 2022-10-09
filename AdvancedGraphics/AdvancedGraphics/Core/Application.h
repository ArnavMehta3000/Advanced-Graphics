#pragma once
#include "Core/Timer.h"
#include "Core/Window.h"
#include "Graphics/Direct3D.h"
#include "Core/GameObject.h"
#include "Core/Camera.h"


#define ENABLE_IMGUI 0
#define DEFAULT_SHADER L"Shaders/Shader.fx"

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
	void OnUpdate(double dt);
	void OnRender();

#if ENABLE_IMGUI
	void OnGui();
#endif // ENABLE_IMGUI

private:
	Timer m_appTimer;
	Window* m_window;

	VertexShader* m_vertexShader;
	PixelShader* m_pixelShader;

	ComPtr<ID3D11Buffer> m_constantBuffer;

	GameObject* m_gamObject;
	Camera* m_camera;

	XMFLOAT4X4 world, view, projection;
};