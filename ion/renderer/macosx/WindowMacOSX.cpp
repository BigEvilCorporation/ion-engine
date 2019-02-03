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
// Description:	MacOSX window
///////////////////////////////////////////////////

#include "core/debug/Debug.h"
#include "renderer/macosx/WindowMacOSX.h"

#include <SDL2/SDL.h>

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
            
            SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
            SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
            
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

		bool WindowMacOSX::Resize(u32 clientAreaWidth, u32 clientAreaHeight, bool adjustForTitle)
		{
			//Set new client area size
			m_clientAreaWidth = clientAreaWidth;
			m_clientAreaHeight = clientAreaHeight;

			m_windowWidth = clientAreaWidth;
			m_windowHeight = clientAreaHeight;
            
            SDL_SetWindowSize(m_windowHandle, clientAreaWidth, clientAreaHeight);

			return true;
		}

		bool WindowMacOSX::SetFullscreen(bool fullscreen, int displayIdx)
		{
			int result = SDL_SetWindowFullscreen(m_windowHandle,
				fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
			bool success = (result == 0);
			if (success) { m_fullscreen = fullscreen; }
			return success;
		}

		void WindowMacOSX::SetTitle(const std::string& title)
		{
            SDL_SetWindowTitle(m_windowHandle, title.c_str());
		}
        
        int WindowMacOSX::GetDisplays(std::vector<Display>& displays) const
        {
            Display display;
            display.size.x = GetDesktopWidth(0);
            display.size.y = GetDesktopHeight(0);
            display.topLeft.x = 0;
            display.topLeft.y = 0;
            display.name = "Display1";
            displays.push_back(display);
        }
        
        u32 WindowMacOSX::GetDesktopWidth(int displayIdx) const
        {
            SDL_DisplayMode displayMode;
            SDL_GetDesktopDisplayMode(0, &displayMode);
            return displayMode.w;
        }
        
        u32 WindowMacOSX::GetDesktopHeight(int displayIdx) const
        {
            SDL_DisplayMode displayMode;
            SDL_GetDesktopDisplayMode(0, &displayMode);
            return displayMode.h;
        }
        
        int WindowMacOSX::GetSupportedResolutions(std::vector<Vector2i>& resolutions, int displayIdx) const
        {
            SDL_DisplayMode mode;
            
            for(int i = 0; i < SDL_GetNumDisplayModes(0); i++)
            {
                if(SDL_GetDisplayMode(0, i, &mode) == 0)
                {
                    resolutions.insert(resolutions.begin(), ion::Vector2i(mode.w, mode.h));
                }
            }
            
            return resolutions.size();
        }
        
        void WindowMacOSX::ShowCursor(bool show)
        {
            SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
        }
        
        bool WindowMacOSX::HasFocus() const
        {
            u32 windowFlags = SDL_GetWindowFlags(m_windowHandle);
            return (windowFlags & SDL_WINDOW_INPUT_FOCUS) != 0;
        }
	}
}
