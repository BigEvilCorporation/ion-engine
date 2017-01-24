///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		WindowMacOSX.cpp
// Date:		24th January 2017
// Authors:		Matt Phillips
// Description:	MacOSX not-really-a-window (yet)
///////////////////////////////////////////////////

#include "core/debug/Debug.h"
#include "renderer/macosx/WindowMacOSX.h"

#include <SDL.h>

namespace ion
{
	namespace render
	{
		Window* Window::Create(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen)
		{
			return new WindowMacOSX(title, clientAreaWidth, clientAreaHeight, fullscreen);
		}

		WindowMacOSX::WindowMacOSX(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen)
			: Window(title, clientAreaWidth, clientAreaHeight, fullscreen)
		{
            SDL_Init(SDL_INIT_VIDEO);
            
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,1);
            
            SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL,1);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
            
            m_windowHandle = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, clientAreaWidth, clientAreaHeight, SDL_WINDOW_OPENGL);
            
			m_deviceContext = m_windowHandle;
		}

		WindowMacOSX::~WindowMacOSX()
		{
		}

		bool WindowMacOSX::Update()
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

		bool WindowMacOSX::Resize(u32 clientAreaWidth, u32 clientAreaHeight)
		{
			//Set new client area size
			m_clientAreaWidth = clientAreaWidth;
			m_clientAreaHeight = clientAreaHeight;

			m_windowWidth = clientAreaWidth;
			m_windowHeight = clientAreaHeight;

			return true;
		}

		void WindowMacOSX::SetFullscreen(bool fullscreen)
		{
		}

		void WindowMacOSX::SetTitle(const std::string& title)
		{

		}
	}
}
