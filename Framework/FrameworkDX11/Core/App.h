#pragma once

#include "structures.h"

#define ENABLE_IMGUI

class Window;
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
	virtual void OnRender(const double dt);
#ifdef ENABLE_IMGUI
	virtual void OnGui(const double dt);
#endif // ENABLE_IMGUI


protected:

private:
	Window* m_window;
	VertexShader* m_vertexShader;
	PixelShader* m_pixelShader;

	ComPtr<ID3D11Buffer> m_constantBuffer;
	ComPtr<ID3D11Buffer> m_lightCBuffer;

};

