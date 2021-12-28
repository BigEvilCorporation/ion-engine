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
#elif defined ION_PLATFORM_SWITCH
typedef void* EGLDisplay;
typedef void* EGLSurface;
typedef void* EGLContext;
typedef void* EGLConfig;
#elif defined ION_PLATFORM_ANDROID
struct ANativeWindow;
typedef void* EGLDisplay;
typedef void* EGLSurface;
typedef void* EGLContext;
typedef void* EGLConfig;
#endif

namespace ion
{
	namespace render
	{
#if defined ION_PLATFORM_WINDOWS
		typedef HWND WindowHandle;
		typedef HDC DeviceContext;
		typedef HGLRC RenderContext;
		static const DeviceContext NullDeviceContext = nullptr;
		static const RenderContext NullRenderContext = nullptr;
#elif defined ION_PLATFORM_MACOSX
        typedef SDL_Window* WindowHandle;
        typedef SDL_Window* DeviceContext;
		typedef SDL_GLContext RenderContext;
		static const DeviceContext NullDeviceContext = nullptr;
		static const RenderContext NullRenderContext = 0;
#elif defined ION_PLATFORM_LINUX
		typedef SDL_Window* WindowHandle;
		typedef SDL_Window* DeviceContext;
		typedef SDL_GLContext RenderContext;
		static const DeviceContext NullDeviceContext = nullptr;
		static const RenderContext NullRenderContext = 0;
#elif defined ION_PLATFORM_RASPBERRYPI
		typedef SDL_Window* WindowHandle;
		typedef SDL_Window* DeviceContext;
		typedef SDL_GLContext RenderContext;
		static const DeviceContext NullDeviceContext = nullptr;
		static const RenderContext NullRenderContext = 0;
#elif defined ION_PLATFORM_DREAMCAST
		typedef int WindowHandle;
		typedef void* DeviceContext;
		typedef int RenderContext;
		static const DeviceContext NullDeviceContext = nullptr;
		static const RenderContext NullRenderContext = 0;
#elif defined ION_PLATFORM_SWITCH
		typedef nn::vi::NativeWindowHandle WindowHandle;
		struct DeviceContext
		{
			DeviceContext() { display = 0; surface = 0; }
			bool operator == (const DeviceContext& rhs) const { return rhs.display == display && rhs.surface == surface; }
			bool operator != (const DeviceContext& rhs) const { return !(*this==rhs); }
			bool operator !() { return !display && !surface; }
			EGLDisplay display;
			EGLSurface surface;
			EGLConfig config;
		};
		typedef EGLContext RenderContext;
		static const DeviceContext NullDeviceContext;
		static const RenderContext NullRenderContext = 0;
#elif defined ION_PLATFORM_ANDROID
		typedef ANativeWindow* WindowHandle;
		struct DeviceContext
		{
			DeviceContext() { display = 0; surface = 0; }
			bool operator == (const DeviceContext& rhs) const { return rhs.display == display && rhs.surface == surface; }
			bool operator != (const DeviceContext& rhs) const { return !(*this == rhs); }
			bool operator !() { return !display && !surface; }
			EGLDisplay display;
			EGLSurface surface;
			EGLConfig config;
		};
		typedef EGLContext RenderContext;
		static const DeviceContext NullDeviceContext;
		static const RenderContext NullRenderContext = 0;
#else
		#error Unsupported platform
#endif
	}
}
