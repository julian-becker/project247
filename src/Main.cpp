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


int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	const auto windowConfig = WindowConfigBuilder{}
		.setHeight(1000)
		.setWidth(1280)
		.setPosX(0)
		.setPosY(500)
		.setStyle(WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN)
		.build();

	Window window(windowConfig);
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
