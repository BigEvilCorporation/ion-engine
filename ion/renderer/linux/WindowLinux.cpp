///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		WindowLinux.cpp
// Date:		14th February 2017
// Authors:		Matt Phillips
// Description:		Linux window
///////////////////////////////////////////////////

#include "core/debug/Debug.h"
#include "renderer/linux/WindowLinux.h"

#include <SDL2/SDL.h>

namespace ion
{
	namespace render
	{
		Window* Window::Create(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen)
		{
			return new WindowLinux(title, clientAreaWidth, clientAreaHeight, fullscreen);
		}

		WindowLinux::WindowLinux(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen)
			: Window(title, clientAreaWidth, clientAreaHeight, fullscreen)
		{
            SDL_Init(SDL_INIT_VIDEO);
            
            SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
            SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
            
            m_windowHandle = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, clientAreaWidth, clientAreaHeight, SDL_WINDOW_OPENGL);
            
			m_deviceContext = m_windowHandle;
		}

		WindowLinux::~WindowLinux()
		{
		}

		bool WindowLinux::Update()
		{
            SDL_Event event;
            
            while(SDL_PollEvent(&event) != 0)
            {
                if(event.type == SDL_QUIT)
                {
                    return false;
                }
                
            }
            
			return true;
		}

		u32 WindowLinux::GetDesktopWidth() const
		{
			SDL_DisplayMode displayMode;
			SDL_GetDesktopDisplayMode(0, &displayMode);
			return displayMode.w;
		}

		u32 WindowLinux::GetDesktopHeight() const
		{
			SDL_DisplayMode displayMode;
			SDL_GetDesktopDisplayMode(0, &displayMode);
			return displayMode.h;
		}

		bool WindowLinux::Resize(u32 clientAreaWidth, u32 clientAreaHeight, bool adjustForTitle)
		{
			//Set new client area size
			m_clientAreaWidth = clientAreaWidth;
			m_clientAreaHeight = clientAreaHeight;

			m_windowWidth = clientAreaWidth;
			m_windowHeight = clientAreaHeight;


			SDL_SetWindowSize(m_windowHandle, clientAreaWidth, clientAreaHeight);

			return true;
		}

		bool WindowLinux::SetFullscreen(bool fullscreen)
		{
			return SDL_SetWindowFullscreen(m_windowHandle, fullscreen ? SDL_WINDOW_FULLSCREEN : 0) == 0;
		}

		void WindowLinux::SetTitle(const std::string& title)
		{
			SDL_SetWindowTitle(m_windowHandle, title.c_str());
		}
	}
}
