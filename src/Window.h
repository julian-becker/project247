///////////////////////////////////////////////////////////
//
// Copyright(c) 2017-2018 Mariusz Bartosik, mariuszbartosik.com
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#ifndef _PROJECT247_WINDOW_H_
#define _PROJECT247_WINDOW_H_

#include <windows.h>

class Window {
public:
	LPTSTR windowClass;	// Window Class
	HGLRC RC;			// Rendering Context
	HDC	DC;				// Device Context
	HWND WND;			// Window
	DWORD style;

	struct Config {
		int width;
		int	height;
		int posX;
		int posY;
		bool windowed;
	} config;

	constexpr Window() noexcept
		: windowClass{}
		, RC{}
		, DC{}
		, WND{}
		, style{}
	{

		config.width = 1024;
		config.height = 720;
		config.posX = CW_USEDEFAULT;
		config.posY = 0;
		config.windowed = true;
		style = WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	}

	int create(HINSTANCE hInstance, int nCmdShow);
	ATOM registerClass(HINSTANCE hInstance) noexcept;
	void adjustSize() noexcept;
	void center() noexcept;
	void render() noexcept;
	void swapBuffers() noexcept;
	void destroy() noexcept;
	static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept;
private:

};
#endif