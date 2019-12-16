///////////////////////////////////////////////////////////
//
// Copyright(c) 2017-2018 Mariusz Bartosik, mariuszbartosik.com
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#define _USE_MATH_DEFINES
#include "Window.h"
#include <cmath>
#include <gl/gl.h>
#include "gl/glext.h"
#include "gl/wglext.h"
#include <tchar.h>
#include <string>
#include <chrono>

static void showMessage(LPCTSTR message) noexcept {

	MessageBox(nullptr, message, _T("Window::create"), MB_ICONERROR);
}

///////////////////////////////////////////////////////////

int Window::create(HINSTANCE hInstance, int nCmdShow) {

	m_windowClass = MAKEINTATOM(registerClass(hInstance));
	if (!m_windowClass) {
		showMessage(_T("registerClass() failed."));
		return 1;
	}

	// create temporary window

	HWND fakeWND = CreateWindow(
		m_windowClass, _T("Fake Window"),
		m_config.style,
		0, 0,						// position x, y
		1, 1,						// width, height
		nullptr, nullptr,					// parent window, menu
		hInstance, nullptr);			// instance, param

	HDC fakeDC = GetDC(fakeWND);	// Device Context

	PIXELFORMATDESCRIPTOR fakePFD;
	ZeroMemory(&fakePFD, sizeof(fakePFD));
	fakePFD.nSize = sizeof(fakePFD);
	fakePFD.nVersion = 1;
	fakePFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	fakePFD.iPixelType = PFD_TYPE_RGBA;
	fakePFD.cColorBits = 32;
	fakePFD.cAlphaBits = 8;
	fakePFD.cDepthBits = 24;

	const int fakePFDID = ChoosePixelFormat(fakeDC, &fakePFD);
	if (fakePFDID == 0) {
		showMessage(_T("ChoosePixelFormat() failed."));
		return 1;
	}

	if (SetPixelFormat(fakeDC, fakePFDID, &fakePFD) == false) {
		showMessage(_T("SetPixelFormat() failed."));
		return 1;
	}

	HGLRC fakeRC = wglCreateContext(fakeDC);	// Rendering Contex

	if (!fakeRC) {
		showMessage(_T("wglCreateContext() failed."));
		return 1;
	}

	if (wglMakeCurrent(fakeDC, fakeRC) == false) {
		showMessage(_T("wglMakeCurrent() failed."));
		return 1;
	}


#pragma warning(push)
#pragma warning(disable:26490)

	// get pointers to functions (or init opengl loader here)

	PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = nullptr;
	wglChoosePixelFormatARB = reinterpret_cast<PFNWGLCHOOSEPIXELFORMATARBPROC>(wglGetProcAddress("wglChoosePixelFormatARB"));
	if (wglChoosePixelFormatARB == nullptr) {
		showMessage(_T("wglGetProcAddress() failed."));
		return 1;
	}

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = nullptr;
	wglCreateContextAttribsARB = reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(wglGetProcAddress("wglCreateContextAttribsARB"));
	if (wglCreateContextAttribsARB == nullptr) {
		showMessage(_T("wglGetProcAddress() failed."));
		return 1;
	}
#pragma warning(pop)

	adjustSize();
	center();

	// create a new window and context
								
	m_WND = CreateWindow(
		m_windowClass, _T("OpenGL Window"),	// class name, window name
		m_config.style,						// styles
		m_config.posX, m_config.posY,		// posx, posy. If x is set to CW_USEDEFAULT y is ignored
		m_config.width, m_config.height,	// width, height
		nullptr, nullptr,					// parent window, menu
		hInstance, nullptr);				// instance, param

	m_DC = GetDC(m_WND);

	const int pixelAttribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
		WGL_SAMPLES_ARB, 4,
		0
	};

	int pixelFormatID; UINT numFormats;
	const bool status = wglChoosePixelFormatARB(m_DC, &pixelAttribs[0], nullptr, 1, &pixelFormatID, &numFormats);

	if (status == false || numFormats == 0) {
		showMessage(_T("wglChoosePixelFormatARB() failed."));
		return 1;
	}

	PIXELFORMATDESCRIPTOR PFD;
	DescribePixelFormat(m_DC, pixelFormatID, sizeof(PFD), &PFD);
	SetPixelFormat(m_DC, pixelFormatID, &PFD);

	const int major_min = 4, minor_min = 0;
	const int contextAttribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major_min,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor_min,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
//		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
		0
	};

	m_RC = wglCreateContextAttribsARB(m_DC, nullptr, &contextAttribs[0]);
	if (!m_RC) {
		showMessage(_T("wglCreateContextAttribsARB() failed."));
		return 1;
	}

	// delete temporary context and window

	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(fakeRC);
	ReleaseDC(fakeWND, fakeDC);
	DestroyWindow(fakeWND);
	if (!wglMakeCurrent(m_DC, m_RC)) {
		showMessage(_T("wglMakeCurrent() failed."));
		return 1;
	}

	// init opengl loader here (extra safe version)
#pragma warning(push)
#pragma warning(disable:26481)
#pragma warning(disable:26490)

	auto glVersion = reinterpret_cast<char const*>(glGetString(GL_VERSION));
#ifdef UNICODE
	std::wstring str(glVersion, glVersion + strlen(glVersion));
#else
	std::string str(glVersion, glVersion + strlen(glVersion));
#endif
#pragma warning(pop)

	SetWindowText(m_WND, str.c_str());
	ShowWindow(m_WND, nCmdShow);

	return 0;
}

///////////////////////////////////////////////////////////

ATOM Window::registerClass(HINSTANCE hInstance) noexcept {

	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WindowProcedure;
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.lpszClassName = _T("Core");

	return RegisterClassEx(&wcex);
}

///////////////////////////////////////////////////////////
// Adjust window's size for non-client area elements
// like border and title bar

void Window::adjustSize() noexcept {

	RECT rect = { 0, 0, m_config.width, m_config.height };
	AdjustWindowRect(&rect, m_config.style, false);
	m_config.width = rect.right - rect.left;
	m_config.height = rect.bottom - rect.top;
}

///////////////////////////////////////////////////////////

void Window::center() noexcept {

	RECT primaryDisplaySize;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &primaryDisplaySize, 0);	// system taskbar and application desktop toolbars not included
	m_config.posX = (primaryDisplaySize.right - m_config.width) / 2;
	m_config.posY = (primaryDisplaySize.bottom - m_config.height) / 2;
}

///////////////////////////////////////////////////////////

void Window::render() noexcept {
	wglMakeCurrent(m_DC, m_RC);

	static const auto start = std::chrono::steady_clock::now();
	const float t = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - start).count();
	glClearColor(0.129f, std::cosf(2.0f * static_cast<float>(M_PI) * t / 5.0f) + 1.0f, std::sinf(2.0f * static_cast<float>(M_PI) * t/5.0f)+1.0f, 1.0f);	// rgb(33,150,243)
	glClear(GL_COLOR_BUFFER_BIT);
}

///////////////////////////////////////////////////////////

void Window::swapBuffers() noexcept {

	SwapBuffers(m_DC);
}

///////////////////////////////////////////////////////////

void Window::destroy() noexcept {

	wglMakeCurrent(nullptr, nullptr);
	if (m_RC) {
		wglDeleteContext(m_RC);
	}
	if (m_DC) {
		ReleaseDC(m_WND, m_DC);
	}
	if (m_WND) {
		DestroyWindow(m_WND);
	}
}

///////////////////////////////////////////////////////////

LRESULT CALLBACK Window::WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept {

	switch (message) {
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE) {
				PostQuitMessage(0);
			}
			break;
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;		// message handled
}
