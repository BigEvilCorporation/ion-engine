///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		WindowMacOSX.h
// Date:		24th January 2017
// Authors:		Matt Phillips
// Description:	MacOSX window
///////////////////////////////////////////////////

#pragma once

#include "renderer/Window.h"

namespace ion
{
	namespace render
	{
		class WindowMacOSX : public Window
		{
		public:
			WindowMacOSX(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen);
			virtual ~WindowMacOSX();

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
			WindowHandle m_windowHandle;
			DeviceContext m_deviceContext;
		};
	}
}
