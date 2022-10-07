#pragma once

#include "Defines.h"
#include "structures.h"

#define ENABLE_IMGUI

class Window;
class GameObject;
struct VertexShader;
struct PixelShader;

class App
{
public:
	App(HINSTANCE hInst);
	~App();

	bool Init();
	void Run();
	void Shutdown();

protected:
	virtual void OnUpdate(const double dt);
	virtual void OnRender();
#ifdef ENABLE_IMGUI
	virtual void OnGui();
#endif // ENABLE_IMGUI


private:
	GameObject* m_gameObject;
	Window* m_window;
	VertexShader* m_vertexShader;
	PixelShader* m_pixelShader;

	ComPtr<ID3D11Buffer> m_constantBuffer;
	ComPtr<ID3D11Buffer> m_lightCBuffer;

};

