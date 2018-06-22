///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Window.cpp
// Date:		12th January 2014
// Authors:		Matt Phillips
// Description:	Window management
///////////////////////////////////////////////////

#include "renderer/Window.h"

namespace ion
{
	namespace render
	{
		Window::Window(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen)
		{
			m_fullscreen = fullscreen;
			m_clientAreaWidth = clientAreaWidth;
			m_clientAreaHeight = clientAreaHeight;
			m_aspectRatio = (float)clientAreaHeight / (float)clientAreaWidth;
		}

		u32 Window::GetWindowWidth() const
		{
			return m_clientAreaWidth;
		}

		u32 Window::GetWindowHeight() const
		{
			return m_clientAreaHeight;
		}

		u32 Window::GetClientAreaWidth() const
		{
			return m_clientAreaWidth;
		}

		u32 Window::GetClientAreaHeight() const
		{
			return m_clientAreaHeight;
		}

		float Window::GetAspectRatio() const
		{
			return m_aspectRatio;
		}

		bool Window::GetFullscreen() const
		{
			return m_fullscreen;
		}
	}
}
