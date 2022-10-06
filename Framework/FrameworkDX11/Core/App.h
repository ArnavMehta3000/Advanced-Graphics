#pragma once

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
	virtual void OnGui(const double dt);

protected:
	Window* m_window;

private:
	VertexShader* m_vertexShader;
	PixelShader* m_pixelShader;

	ComPtr<ID3D11Buffer> m_constantBuffer;
};

