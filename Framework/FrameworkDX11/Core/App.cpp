#include "pch.h"
#include "App.h"
#include "Window.h"

App::App(HINSTANCE hInst)
	:
	m_window(nullptr)
{
	m_window = new Window(hInst, 1280, 720);
}

App::~App()
{
}

bool App::Init()
{
	return true;
}

void App::Run()
{
}

void App::Shutdown()
{
}
