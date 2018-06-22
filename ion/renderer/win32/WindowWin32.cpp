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
			int desktopWidth = GetDesktopWidth();
			int desktopHeight = GetDesktopHeight();

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

		u32 WindowWin32::GetDesktopWidth() const
		{
			return GetSystemMetrics(SM_CXSCREEN);
		}

		u32 WindowWin32::GetDesktopHeight() const
		{
			return GetSystemMetrics(SM_CYSCREEN);
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

		bool WindowWin32::SetFullscreen(bool fullscreen)
		{
			if (fullscreen)
			{
				//Turn off window region without redraw
				SetWindowRgn(m_windowHandle, 0, false);

				//Get current window settings
				DEVMODE deviceMode;
				EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &deviceMode);

				//Not changing anything else 
				deviceMode.dmFields = 0;

				if (ChangeDisplaySettings(&deviceMode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL)
				{
					//Switch off the title bar
					DWORD style = GetWindowLong(m_windowHandle, GWL_STYLE);
					style &= ~WS_CAPTION;
					SetWindowLong(m_windowHandle, GWL_STYLE, style);

					//Move the window to (0,0)
					SetWindowPos(m_windowHandle, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
					InvalidateRect(m_windowHandle, 0, true);

					//Set new client area without titlebar
					m_clientAreaWidth = m_windowWidth;
					m_clientAreaHeight = m_windowHeight;

					m_fullscreen = true;

					return true;
				}
			}
			else
			{
				debug::Error("NOT IMPLEMENTED");
			}

			return false;
		}

		void WindowWin32::SetTitle(const std::string& title)
		{
			SetWindowText(m_windowHandle, title.c_str());
		}

		LRESULT CALLBACK WindowWin32::WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch(message)
			{
				case WM_CLOSE:
					DestroyWindow(hwnd);
				break;
				case WM_DESTROY:
					PostQuitMessage(0);
				break;
				default:
					return DefWindowProc(hwnd, message, wParam, lParam);
			}

			return 0;
		}
	}
}