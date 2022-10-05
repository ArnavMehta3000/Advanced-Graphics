#pragma once

class Window;

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

protected:
	UniquePtr<Window> m_window;
};

