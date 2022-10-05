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

private:
	Window* m_window;
};

