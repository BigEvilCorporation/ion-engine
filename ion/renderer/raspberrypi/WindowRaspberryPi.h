///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		WindowLinux.h
// Date:		15th February 2017
// Authors:		Matt Phillips
// Description:	Raspberry Pi window
///////////////////////////////////////////////////

#pragma once

#include "renderer/Window.h"

namespace ion
{
	namespace render
	{
		class WindowRaspberryPi : public Window
		{
		public:
			WindowRaspberryPi(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen);
			virtual ~WindowRaspberryPi();

			virtual bool Update();
			virtual bool Resize(u32 clientAreaWidth, u32 clientAreaHeight);
			virtual void SetFullscreen(bool fullscreen);
			virtual void SetTitle(const std::string& title);

			virtual DeviceContext GetDeviceContext() const { return m_deviceContext; }
			virtual WindowHandle GetWindowHandle() const { return m_windowHandle; }

		protected:
			WindowHandle m_windowHandle;
			DeviceContext m_deviceContext;
		};
	}
}
