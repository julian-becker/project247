///////////////////////////////////////////////////////////
//
// Copyright(c) 2017-2018 Mariusz Bartosik, mariuszbartosik.com
// Subject to the MIT license, see LICENSE file.
//
///////////////////////////////////////////////////////////

#ifndef _PROJECT247_WINDOW_H_
#define _PROJECT247_WINDOW_H_

#include <windows.h>
#include <utility>

struct WindowConfig {
	int width;
	int	height;
	int posX;
	int posY;
	DWORD style;
};

class WindowConfigBuilder {
	WindowConfig m_config{};
public:
	constexpr auto setWidth(int width) noexcept -> WindowConfigBuilder& {
		m_config.width = width;
		return *this;
	}
	constexpr auto setHeight(int height) noexcept -> WindowConfigBuilder& {
		m_config.height = height;
		return *this;
	}
	constexpr auto setPosX(int posX) noexcept  -> WindowConfigBuilder& {
		m_config.posX = posX;
		return *this;
	}
	constexpr auto setPosY(int posY) noexcept -> WindowConfigBuilder& {
		m_config.posY = posY;
		return *this;
	}
	constexpr auto setStyle(DWORD style) noexcept -> WindowConfigBuilder& {
		m_config.style = style;
		return *this;
	}
	constexpr auto build() const -> WindowConfig {
		return m_config;
	}
};

class Window {
public:
	LPTSTR m_windowClass;	// Window Class
	HGLRC m_RC;			// Rendering Context
	HDC	m_DC;				// Device Context
	HWND m_WND;			// Window
	WindowConfig m_config;


	constexpr Window(WindowConfig config) noexcept
		: m_windowClass{}
		, m_RC{}
		, m_DC{}
		, m_WND{}
		, m_config{std::move(config)}
	{
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