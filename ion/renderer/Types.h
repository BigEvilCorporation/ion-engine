///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		types.h
// Date:		13th June 2016
// Authors:		Matt Phillips
// Description:	Renderer types
///////////////////////////////////////////////////

#pragma once

#if defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_LINUX || defined ION_PLATFORM_RASPBERRYPI
struct SDL_Window;
typedef void* SDL_GLContext;
#endif

namespace ion
{
	namespace render
	{
#if defined ION_PLATFORM_WINDOWS
		typedef HWND WindowHandle;
		typedef HDC DeviceContext;
		typedef HGLRC RenderContext;
#elif defined ION_PLATFORM_MACOSX
        typedef SDL_Window* WindowHandle;
        typedef SDL_Window* DeviceContext;
		typedef SDL_GLContext RenderContext;
#elif defined ION_PLATFORM_LINUX
		typedef SDL_Window* WindowHandle;
		typedef SDL_Window* DeviceContext;
		typedef SDL_GLContext RenderContext;
#elif defined ION_PLATFORM_RASPBERRYPI
		typedef SDL_Window* WindowHandle;
		typedef SDL_Window* DeviceContext;
		typedef SDL_GLContext RenderContext;
#elif defined ION_PLATFORM_DREAMCAST
		typedef int WindowHandle;
		typedef void* DeviceContext;
		typedef int RenderContext;
#else
		#error Unsupported platform
#endif
	}
}
