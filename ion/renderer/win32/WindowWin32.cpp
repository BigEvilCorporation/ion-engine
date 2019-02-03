///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		WindowWin32.cpp
// Date:		12th January 2014
// Authors:		Matt Phillips
// Description:	Win32 window
///////////////////////////////////////////////////

#pragma once

#include "core/debug/Debug.h"
#include "renderer/win32/WindowWin32.h"

namespace ion
{
	namespace render
	{
		DeviceContext WindowWin32::s_winProcDC = 0;

		Window* Window::Create(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen)
		{
			return new WindowWin32(title, clientAreaWidth, clientAreaHeight, fullscreen);
		}

		WindowWin32::WindowWin32(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen)
			: Window(title, clientAreaWidth, clientAreaHeight, fullscreen)
		{
			std::string windowClassName = "WindowClass_";
			windowClassName += title;

			m_hInstance = GetModuleHandle(NULL);

			//Register window class
			WNDCLASSEX windowClass = {0};
			windowClass.cbSize        = sizeof(WNDCLASSEX);
			windowClass.style         = 0;
			windowClass.lpfnWndProc   = WindowProcedure;
			windowClass.cbClsExtra    = 0;
			windowClass.cbWndExtra    = 0;
			windowClass.hInstance     = m_hInstance;
			windowClass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
			windowClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
			windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
			windowClass.lpszMenuName  = NULL;
			windowClass.lpszClassName = windowClassName.c_str();
			windowClass.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);

			if(!RegisterClassEx(&windowClass))
			{
				debug::Error("Could not register window class");
			}

			//Get desktop resolution
			int desktopWidth = GetDesktopWidth(0);
			int desktopHeight = GetDesktopHeight(0);

			int x = 0;
			int y = 0;

			//Setup window style flags and fullscreen mode
			if(fullscreen)
			{
				m_windowStyle = WS_POPUP;
				m_windowStyleEx = WS_EX_APPWINDOW;

				DEVMODE deviceMode;
				EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &deviceMode);

				//Use existing desktop size
				m_windowWidth = deviceMode.dmPelsWidth;
				m_windowHeight = deviceMode.dmPelsHeight;
				m_clientAreaWidth = deviceMode.dmPelsWidth;
				m_clientAreaHeight = deviceMode.dmPelsHeight;

				if(ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
				{
					ion::debug::Log("WindowWin32::WindowWin32() - Failed to set fullscreen mode");

					//Fall back to windowed mode
					fullscreen = false;
				}
			}
			else
			{
				m_windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU;
				m_windowStyleEx = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

				//Set client area size
				m_clientAreaWidth = clientAreaWidth;
				m_clientAreaHeight = clientAreaHeight;

				//Setup window rect
				RECT windowRect = { 0 };
				windowRect.right = (long)clientAreaWidth;
				windowRect.bottom = (long)clientAreaHeight;

				//Adjust rect to account for window border
				AdjustWindowRectEx(&windowRect, m_windowStyle, FALSE, m_windowStyleEx);

				m_windowWidth = windowRect.right - windowRect.left;
				m_windowHeight = windowRect.bottom - windowRect.top;

				//Calculate centre position
				x = (desktopWidth / 2) - (m_windowWidth / 2);
				y = (desktopHeight / 2) - (m_windowHeight / 2);
			}

			//Create window
			m_windowHandle = CreateWindowEx(m_windowStyleEx, windowClassName.c_str(), title.c_str(), m_windowStyle, x, y, m_windowWidth, m_windowHeight, NULL, NULL, m_hInstance, NULL);

			if(!m_windowHandle)
			{
				debug::Error("Could not create window");
			}

			//Get device context
			m_deviceContext = GetDC(m_windowHandle);

			//Create pixel format descriptor
			PIXELFORMATDESCRIPTOR pixelFormatDesc = { 0 };
			pixelFormatDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
			pixelFormatDesc.nVersion = 1;
			pixelFormatDesc.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_SUPPORT_COMPOSITION | PFD_DOUBLEBUFFER;
			pixelFormatDesc.iPixelType = PFD_TYPE_RGBA;
			pixelFormatDesc.cColorBits = 32;
			pixelFormatDesc.cDepthBits = 24;
			pixelFormatDesc.cStencilBits = 8;
			pixelFormatDesc.iLayerType = PFD_MAIN_PLANE;

			//Check pixel format
			int pixelFormat = ChoosePixelFormat(m_deviceContext, &pixelFormatDesc);
			if(!pixelFormat)
			{
				debug::Error("Invalid pixel format");
			}

			//Set pixel format
			if(!SetPixelFormat(m_deviceContext, pixelFormat, &pixelFormatDesc))
			{
				debug::Error("Could not set pixel format");
			}

			//Set static DC for window procedure
			s_winProcDC = m_deviceContext;

			//Show and update
			ShowWindow(m_windowHandle, SW_SHOW);
			UpdateWindow(m_windowHandle);
		}

		WindowWin32::~WindowWin32()
		{
		}

		bool WindowWin32::Update()
		{
			MSG message;
			bool quit = false;

			while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE) > 0)
			{
				quit |= (message.message == WM_QUIT);

				TranslateMessage(&message);
				DispatchMessage(&message);
			}

			return !quit;
		}

		u32 WindowWin32::GetDesktopWidth(int displayIdx) const
		{
			std::vector<Display> displays;
			GetDisplays(displays);

			if (displayIdx < displays.size())
			{
				return displays[displayIdx].size.x;
			}

			return GetSystemMetrics(SM_CXSCREEN);
		}

		u32 WindowWin32::GetDesktopHeight(int displayIdx) const
		{
			std::vector<Display> displays;
			GetDisplays(displays);

			if (displayIdx < displays.size())
			{
				return displays[displayIdx].size.y;
			}

			return GetSystemMetrics(SM_CYSCREEN);
		}

		int WindowWin32::GetDisplays(std::vector<Display>& displays) const
		{
			std::vector<Win32Display> win32Displays;
			EnumDisplayMonitors(NULL, NULL, WindowWin32::DisplayEnumProcedure, (LPARAM)&win32Displays);

			displays.clear();

			for (int i = 0; i < win32Displays.size(); i++)
			{
				Display display;
				display.size = Vector2i(std::abs(win32Displays[i].monitorRect.left - win32Displays[i].monitorRect.right), std::abs(win32Displays[i].monitorRect.top - win32Displays[i].monitorRect.bottom));
				display.topLeft = Vector2i(win32Displays[i].workspaceRect.left, win32Displays[i].workspaceRect.top);
				display.name = win32Displays[i].name;
				display.index = i;
				displays.push_back(display);
			}

			return displays.size();
		}

		int WindowWin32::GetSupportedResolutions(std::vector<Vector2i>& resolutions, int displayIdx) const
		{
			DEVMODE deviceMode = { 0 };
			deviceMode.dmSize = sizeof(deviceMode);
			for (int i = 0; EnumDisplaySettings(NULL, i, &deviceMode) != 0; i++)
			{
				resolutions.push_back(Vector2i(deviceMode.dmPelsWidth, deviceMode.dmPelsHeight));
			}

			return resolutions.size();
		}

		bool WindowWin32::Resize(u32 clientAreaWidth, u32 clientAreaHeight, bool adjustForTitle)
		{
			//Set new client area size
			m_clientAreaWidth = clientAreaWidth;
			m_clientAreaHeight = clientAreaHeight;

			//Setup window rect
			RECT windowRect = { 0 };
			windowRect.right = (long)clientAreaWidth;
			windowRect.bottom = (long)clientAreaHeight;

			//Adjust rect to account for window border
			if (adjustForTitle)
			{
				AdjustWindowRectEx(&windowRect, m_windowStyle, FALSE, m_windowStyleEx);
			}

			m_windowWidth = windowRect.right - windowRect.left;
			m_windowHeight = windowRect.bottom - windowRect.top;

			//Get desktop resolution
			int desktopWidth = GetSystemMetrics(SM_CXSCREEN);
			int desktopHeight = GetSystemMetrics(SM_CYSCREEN);

			//Calculate centre position
			int x = (desktopWidth / 2) - (m_windowWidth / 2);
			int y = (desktopHeight / 2) - (m_windowHeight / 2);

			return SetWindowPos(m_windowHandle, NULL, x, y, m_windowWidth, m_windowHeight, SWP_NOCOPYBITS) != 0;
		}

		bool WindowWin32::SetFullscreen(bool fullscreen, int displayIdx)
		{
			if (fullscreen)
			{
				std::vector<Win32Display> displays;
				EnumDisplayMonitors(NULL, NULL, WindowWin32::DisplayEnumProcedure, (LPARAM)&displays);

				if (displayIdx < displays.size())
				{
					const Win32Display& display = displays[displayIdx];

					//Turn off window region redraw
					SetWindowRgn(m_windowHandle, 0, false);

					//Get current window settings
					DEVMODE deviceMode;
					EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &deviceMode);

					//Set fullscreen-safe settings
					deviceMode.dmPelsWidth = GetDeviceCaps(m_deviceContext, HORZRES);
					deviceMode.dmPelsHeight = GetDeviceCaps(m_deviceContext, VERTRES);
					deviceMode.dmBitsPerPel = GetDeviceCaps(m_deviceContext, BITSPIXEL);
					deviceMode.dmDisplayFrequency = GetDeviceCaps(m_deviceContext, VREFRESH);
					deviceMode.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;

					//Set fullscreen mode
					if (ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
					{
						//Set fullscreen window style
						SetWindowLongPtr(m_windowHandle, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
						SetWindowLongPtr(m_windowHandle, GWL_STYLE, WS_POPUP | WS_VISIBLE);

						//Move the window to (0,0)
						SetWindowPos(m_windowHandle, 0, display.monitorRect.left, display.monitorRect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
						InvalidateRect(m_windowHandle, 0, true);

						//Set new client area without titlebar
						m_clientAreaWidth = m_windowWidth;
						m_clientAreaHeight = m_windowHeight;

						UpdateWindow(m_windowHandle);

						m_fullscreen = true;

						return true;
					}
				}
			}
			else
			{
				//Turn on window region redraw
				SetWindowRgn(m_windowHandle, 0, true);

				//Restore from fullscreen mode
				if (ChangeDisplaySettings(NULL, CDS_RESET) == DISP_CHANGE_SUCCESSFUL)
				{
					//Set non-fullscreen window style
					SetWindowLongPtr(m_windowHandle, GWL_EXSTYLE, WS_EX_LEFT);
					SetWindowLongPtr(m_windowHandle, GWL_STYLE, WS_OVERLAPPED | WS_VISIBLE | WS_CAPTION);

					//Centre the window
					int x = (GetDesktopWidth(displayIdx) / 2) - (m_windowWidth / 2);
					int y = (GetDesktopHeight(displayIdx) / 2) - (m_windowHeight / 2);
					SetWindowPos(m_windowHandle, 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
					InvalidateRect(m_windowHandle, 0, true);

					//Set new client area
					RECT windowRect = { 0 };
					windowRect.right = (long)m_clientAreaWidth;
					windowRect.bottom = (long)m_clientAreaHeight;

					//Adjust rect to account for window border
					AdjustWindowRectEx(&windowRect, m_windowStyle, FALSE, m_windowStyleEx);

					m_windowWidth = windowRect.right - windowRect.left;
					m_windowHeight = windowRect.bottom - windowRect.top;

					UpdateWindow(m_windowHandle);

					m_fullscreen = false;

					return true;
				}
			}

			return false;
		}

		void WindowWin32::SetTitle(const std::string& title)
		{
			SetWindowText(m_windowHandle, title.c_str());
		}

		void WindowWin32::ShowCursor(bool show)
		{
			::ShowCursor(show);
		}

		bool WindowWin32::HasFocus() const
		{
			return GetActiveWindow() == m_windowHandle;
		}

		LRESULT CALLBACK WindowWin32::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch(message)
			{
				case WM_PAINT:
					SwapBuffers(s_winProcDC);
					break;
				case WM_CLOSE:
					DestroyWindow(hwnd);
					break;
				case WM_DESTROY:
					PostQuitMessage(0);
					break;
			}

			return DefWindowProc(hwnd, message, wParam, lParam);
		}

		BOOL CALLBACK WindowWin32::DisplayEnumProcedure(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
		{
			MONITORINFOEXA info;
			info.cbSize = sizeof(MONITORINFOEXA);
			GetMonitorInfo(hMonitor, &info);

			Win32Display display;
			display.handle = hMonitor;
			display.hdc = hdcMonitor;
			display.monitorRect = *lprcMonitor;
			display.workspaceRect = info.rcWork;
			display.name = info.szDevice;

			std::vector<Win32Display>* displays = (std::vector<Win32Display>*)dwData;
			displays->push_back(display);

			return 1;
		}
	}
}