///////////////////////////////////////////////////////////
//
// Copyright(c) 2017-2018 Mariusz Bartosik, mariuszbartosik.com
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include "Window.h"

static Window window;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {

	if (window.create(hInstance, nCmdShow) != 0) {
		PostQuitMessage(1);
	}
	 
	MSG msg{};
	bool active = true;
	while (active) {
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				active = false;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		window.render();
		window.swapBuffers();
	}
	window.destroy();

	return static_cast<int>(msg.wParam);
}
