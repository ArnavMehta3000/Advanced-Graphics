#include "pch.h"
#include "App.h"
#include "Core/Window.h"
#include "Core/Direct3D.h"

App::App(HINSTANCE hInst)
	:
	m_window(nullptr)
{
	m_window = std::make_unique<Window>(hInst, 1280, 720);
}

App::~App()
{
}

bool App::Init()
{
	if (!D3D->Init(m_window->GetHandle(), true))
		return false;

	return true;
}

void App::Run()
{
	while (m_window->ProcessMessages())
	{
		OnUpdate(0.0);
		OnRender(0.0);
	}
}

void App::Shutdown()
{
	D3D->Shutdown();
}

void App::OnUpdate(double dt)
{
	dt;
}

void App::OnRender(double dt)
{
	dt;
}