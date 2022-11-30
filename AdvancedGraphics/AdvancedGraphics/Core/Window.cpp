#include "pch.h"
#include "Window.h"
#include "Core/Core.h"

static const LPCWSTR s_className = L"Framework";

Window::Window(HINSTANCE hInst, UINT width, UINT height)
	:
	m_hInstance(hInst),
	m_windowWidth(0),
	m_windowHeight(0),
	m_clientWidth(width),
	m_clientHeight(height)
{


	// Register class
	CREATE_ZERO(WNDCLASSEX, wcex);
	wcex.cbSize        = sizeof(WNDCLASSEX);
	wcex.style         = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc   = MessageRouter;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = hInst;
	wcex.hIcon         = nullptr;
	wcex.hIconSm       = nullptr;
	wcex.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName  = nullptr;
	wcex.lpszClassName = s_className;

	RegisterClassEx(&wcex);

	// Removing thick frame (responsible for resizing) and maximize box 
	DWORD style = WS_OVERLAPPEDWINDOW & ~WS_MAXIMIZEBOX & ~WS_THICKFRAME;
	RECT rc{ 0, 0, static_cast<LONG>(m_clientWidth), static_cast<LONG>(m_clientHeight) };
	AdjustWindowRect(&rc, style, FALSE);

	m_windowWidth = rc.right - rc.left;
	m_windowHeight = rc.bottom - rc.top;

	m_hWnd = CreateWindow(
		s_className,
		L"Advanced Graphics Demo",
		style,
		300, 300,  // Position (x,y)
		m_windowWidth, m_windowHeight,
		nullptr,
		nullptr,
		hInst,
		this
	);

	if (!m_hWnd)
		__debugbreak();


	ShowWindow(m_hWnd, SW_SHOWMAXIMIZED);
	SetFocus(m_hWnd);


	m_keyboard = std::make_unique<Keyboard>();
	m_mouse = std::make_unique<Mouse>();
	m_mouse->SetWindow(m_hWnd);
}

Window::~Window()
{
	UnregisterClass(s_className, m_hInstance);
}

bool Window::ProcessMessages()
{
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// Process input here
			m_kbState = m_keyboard->GetState();
			m_mouseState = m_mouse->GetState();

			m_mouse->SetMode(m_mouseState.rightButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);
			m_mouse->ResetScrollWheelValue();

			return true;
		}
	}
	return false;
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

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Window::MyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_ACTIVATEAPP:
		Keyboard::ProcessMessage(msg, wParam, lParam);
		Mouse::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_ACTIVATE:
	case WM_INPUT:
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_MOUSEHOVER:
		Mouse::ProcessMessage(msg, wParam, lParam);

	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		Keyboard::ProcessMessage(msg, wParam, lParam);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}

	return 0;
}



HWND Window::GetHandle() { return m_hWnd; }

const UINT Window::GetWindowWidth() const noexcept { return m_windowWidth; }

const UINT Window::GetWindowHeight() const noexcept { return m_windowHeight; }

const UINT Window::GetClientWidth() const noexcept { return m_clientWidth; }

const UINT Window::GetClientHeight() const noexcept { return m_clientHeight; }