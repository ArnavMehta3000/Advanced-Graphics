#include "pch.h"
#include "Core/Core.h"
#include "Core/Application.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	UNREFERENCED_PARAMETER(nCmdShow);

	Application* app = new Application(hInstance, 1280, 720);
	
	if (!app->Init()) LOG("Failed to create application");
	app->Run();
	app->Shutdown();
	
	delete app;
	
	return 0;
}