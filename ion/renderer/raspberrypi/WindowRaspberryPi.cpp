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
// Description:	Raspberry Pi window
///////////////////////////////////////////////////

#include "core/debug/Debug.h"
#include "renderer/raspberrypi/WindowRaspberryPi.h"

#include <SDL2/SDL.h>
#include <GLES2/gl2.h>

namespace ion
{
	namespace render
	{
		Window* Window::Create(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen)
		{
			return new WindowRaspberryPi(title, clientAreaWidth, clientAreaHeight, fullscreen);
		}

		WindowRaspberryPi::WindowRaspberryPi(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen)
			: Window(title, clientAreaWidth, clientAreaHeight, fullscreen)
		{
            SDL_Init(SDL_INIT_VIDEO);
            
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
            SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 1 );
            SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );

			m_windowHandle = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, clientAreaWidth, clientAreaHeight, SDL_WINDOW_OPENGL);

			if(!m_windowHandle)
			{
				ion::debug::error << "Could not create SDL window: " << SDL_GetError() << ion::debug::end;
			}

			SDL_Renderer* renderer = SDL_CreateRenderer(m_windowHandle, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

			const GLubyte* version = glGetString(GL_VERSION);
			if(version)
			{
				ion::debug::Log((const char*)version);
			}
			else
			{
				ion::debug::Error("WindowRaspberryPi::WindowRaspberryPi() - Could not determine OpenGL ES version");
			}

			m_deviceContext = m_windowHandle;
		}

		WindowRaspberryPi::~WindowRaspberryPi()
		{
		}

		bool WindowRaspberryPi::Update()
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

		bool WindowRaspberryPi::Resize(u32 clientAreaWidth, u32 clientAreaHeight)
		{
			//Set new client area size
			m_clientAreaWidth = clientAreaWidth;
			m_clientAreaHeight = clientAreaHeight;

			m_windowWidth = clientAreaWidth;
			m_windowHeight = clientAreaHeight;

			return true;
		}

		void WindowRaspberryPi::SetFullscreen(bool fullscreen)
		{
		}

		void WindowRaspberryPi::SetTitle(const std::string& title)
		{

		}
	}
}
