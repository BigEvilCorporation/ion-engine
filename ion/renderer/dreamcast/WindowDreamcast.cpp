///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		WindowDreamcast.cpp
// Date:		3rd January 2017
// Authors:		Matt Phillips
// Description:	Dreamcast not-really-a-window
///////////////////////////////////////////////////

#include "core/debug/Debug.h"
#include "renderer/dreamcast/WindowDreamcast.h"

namespace ion
{
	namespace render
	{
		Window* Window::Create(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen)
		{
			return new WindowDreamcast(title, clientAreaWidth, clientAreaHeight, fullscreen);
		}

		WindowDreamcast::WindowDreamcast(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen)
			: Window(title, clientAreaWidth, clientAreaHeight, fullscreen)
		{
			m_windowHandle = 0;
			m_deviceContext = 0;
		}

		WindowDreamcast::~WindowDreamcast()
		{
		}

		bool WindowDreamcast::Update()
		{
			return true;
		}

		bool WindowDreamcast::Resize(u32 clientAreaWidth, u32 clientAreaHeight)
		{
			//Set new client area size
			m_clientAreaWidth = clientAreaWidth;
			m_clientAreaHeight = clientAreaHeight;

			m_windowWidth = clientAreaWidth;
			m_windowHeight = clientAreaHeight;

			return true;
		}

		void WindowDreamcast::SetFullscreen(bool fullscreen)
		{
		}

		void WindowDreamcast::SetTitle(const std::string& title)
		{

		}
	}
}