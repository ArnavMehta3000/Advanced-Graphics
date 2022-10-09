#pragma once

#include "Defines.h"
#include "structures.h"

//#define ENABLE_IMGUI

class Window;
class GameObject;
struct VertexShader;
struct PixelShader;
struct ConstantBuffer;

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
	void CalculateLighting();

private:
	GameObject*     m_gameObject;
	Window*         m_window;
	VertexShader*   m_vertexShader;
	PixelShader*    m_pixelShader;

	ConstantBuffer* m_constantBuffer;
	ConstantBuffer* m_lightCBuffer;


	sm::Matrix world;
	sm::Matrix view;
	sm::Matrix projection;
};

