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
#include "renderer/window/dreamcast/WindowDreamcast.h"

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

		bool WindowDreamcast::Resize(u32 clientAreaWidth, u32 clientAreaHeight, bool adjustForTitle)
		{
			//Set new client area size
			m_clientAreaWidth = clientAreaWidth;
			m_clientAreaHeight = clientAreaHeight;

			m_windowWidth = clientAreaWidth;
			m_windowHeight = clientAreaHeight;

			return true;
		}

		bool WindowDreamcast::SetFullscreen(bool fullscreen, int displayIdx)
		{
			return true;
		}

		void WindowDreamcast::SetTitle(const std::string& title)
		{

		}

		void WindowDreamcast::ShowCursor(bool show)
		{

		}

		bool WindowDreamcast::HasFocus() const
		{
			return true;
		}

		u32 WindowDreamcast::GetDesktopWidth(int displayIdx) const
		{
			return 640;
		}

		u32 WindowDreamcast::GetDesktopHeight(int displayIdx) const
		{
			return 480;
		}

		int WindowDreamcast::GetDisplays(std::vector<Display>& displays) const
		{
			Display display;
			display.index = 0;
			display.name = "Main";
			display.size.x = m_windowWidth;
			display.size.y = m_windowHeight;
			displays.push_back(display);

			return displays.size();
		}

		int WindowDreamcast::GetSupportedResolutions(std::vector<Vector2i>& resolutions, int displayIdx) const
		{
			// DREAMCAST_TODO
			resolutions.push_back(Vector2i(640, 480));
			return resolutions.size();
		}
	}
}