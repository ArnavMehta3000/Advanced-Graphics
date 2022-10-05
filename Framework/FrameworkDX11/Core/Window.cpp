#include "pch.h"
#include "Window.h"
#include <Resource.h>

Window::Window(HINSTANCE hInst, UINT width, UINT height)
	:
	m_hInstance(hInst),
	m_windowWidth(0),
	m_windowHeight(0),
	m_clientWidth(width),
	m_clientHeight(height)
{

	const LPCWSTR c_className = L"Framework";

	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MessageRouter;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = LoadIcon(hInst, (LPCTSTR)IDI_TUTORIAL1);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = c_className;
	wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);

	RegisterClassEx(&wcex);

	DWORD style = WS_OVERLAPPEDWINDOW;
	RECT rc{ 0, 0, m_clientWidth, m_clientHeight };
	AdjustWindowRect(&rc, style, FALSE);
	
	m_windowWidth = rc.right - rc.left;
	m_windowHeight = rc.bottom - rc.top;

	m_hWnd = CreateWindow(
		c_className,
		L"Advanced Graphics Demo",
		style,
		CW_USEDEFAULT, CW_USEDEFAULT,  // Position (x,y)
		m_windowWidth, m_windowHeight,
		nullptr,
		nullptr,
		hInst,
		this
	);

	// TODO: Add error logging here
	if (!m_hWnd)
	{
		auto e = GetLastError();
		__debugbreak();
	}


	ShowWindow(m_hWnd, SW_SHOW);
}

Window::~Window()
{
}

LRESULT Window::MessageRouter(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* window;

	if (msg == WM_CREATE)
	{
		window = (Window*)((LPCREATESTRUCT)lParam)->lpCreateParams;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);
	}
	else
	{
		window = (Window*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
	}

	return window->MyWndProc(hWnd, msg, wParam, lParam);
}

LRESULT Window::MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (msg)
	{
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = m_windowWidth / 2;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = m_windowHeight / 2;
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}

const UINT Window::GetWindowWidth() const noexcept
{
	return m_windowWidth;
}

const UINT Window::GetWindowHeight() const noexcept
{
	return m_windowHeight;
}

const UINT Window::GetClientWidth() const noexcept
{
	return m_clientWidth;
}

const UINT Window::GetClientHeight() const noexcept
{
	return m_clientHeight;
}
