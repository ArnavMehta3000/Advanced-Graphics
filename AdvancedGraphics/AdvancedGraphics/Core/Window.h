#pragma once
#include "Mouse.h"
#include "Keyboard.h"

// Input handling from 
// https://github.com/microsoft/DirectXTK/wiki/Mouse-and-keyboard-input

class Window
{
public:
	Window(HINSTANCE hInst, UINT width, UINT height);
	~Window();

	bool ProcessMessages();

	static LRESULT CALLBACK MessageRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND GetHandle();

	const DirectX::Keyboard::State& GetKbState() { return m_kbState; }
	const DirectX::Mouse::State& GetMouseState() { return m_mouseState; }

	const UINT GetWindowWidth() const noexcept;
	const UINT GetWindowHeight() const noexcept;

	const UINT GetClientWidth() const noexcept;
	const UINT GetClientHeight() const noexcept;

private:
	HWND m_hWnd;
	HINSTANCE m_hInstance;

	UINT m_windowWidth, m_windowHeight;
	UINT m_clientWidth, m_clientHeight;

	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;

	DirectX::Keyboard::State m_kbState;
	DirectX::Mouse::State m_mouseState;
};