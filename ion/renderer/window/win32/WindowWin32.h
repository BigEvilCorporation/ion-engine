///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		WindowWin32.h
// Date:		12th January 2014
// Authors:		Matt Phillips
// Description:	Win32 window
///////////////////////////////////////////////////

#pragma once

#include "core/Platform.h"
#include "renderer/Window.h"

namespace ion
{
	namespace render
	{
		class WindowWin32 : public Window
		{
		public:
			WindowWin32(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen);
			virtual ~WindowWin32();

			virtual bool Update();
			virtual bool Resize(u32 clientAreaWidth, u32 clientAreaHeight, bool adjustForTitle);
			virtual bool SetFullscreen(bool fullscreen, int displayIdx);
			virtual void SetTitle(const std::string& title);
			virtual void ShowCursor(bool show);
			virtual bool HasFocus() const;

			virtual u32 GetDesktopWidth(int displayIdx) const;
			virtual u32 GetDesktopHeight(int displayIdx) const;

			virtual int GetDisplays(std::vector<Display>& displays) const;
			virtual int GetSupportedResolutions(std::vector<Vector2i>& resolutions, int displayIdx) const;

			virtual DeviceContext GetDeviceContext() const { return m_deviceContext; }
			virtual WindowHandle GetWindowHandle() const { return m_windowHandle; }

		protected:
			struct Win32Display
			{
				HMONITOR handle;
				HDC hdc;
				RECT monitorRect;
				RECT workspaceRect;
				std::string name;
			};

			static LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
			static BOOL CALLBACK DisplayEnumProcedure(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);
			static DeviceContext s_winProcDC;

			HINSTANCE m_hInstance;
			HWND m_windowHandle;
			DWORD m_windowStyle;
			DWORD m_windowStyleEx;
			DeviceContext m_deviceContext;
		};
	}
}