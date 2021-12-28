///////////////////////////////////////////////////
// (c) 2019 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		WindowAndroid.cpp
// Date:		2nd September 2019
// Authors:		Matt Phillips
// Description:	Android not-really-a-window
///////////////////////////////////////////////////

#include "core/debug/Debug.h"
#include "renderer/window/Android/WindowAndroid.h"

#include <android/native_window.h>

namespace ion
{
	namespace render
	{
		Window* Window::Create(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen)
		{
			return new WindowAndroid(title, clientAreaWidth, clientAreaHeight, fullscreen);
		}

		WindowAndroid::WindowAndroid(const std::string& title, u32 clientAreaWidth, u32 clientAreaHeight, bool fullscreen)
			: Window(title, clientAreaWidth, clientAreaHeight, fullscreen)
		{
			m_windowHandle = WindowHandle();
			m_deviceContext = DeviceContext();

			//Create EGL display
			m_deviceContext.display = ::eglGetDisplay(EGL_DEFAULT_DISPLAY);
			debug::Assert(m_deviceContext.display != NULL, "eglGetDisplay failed.");

			EGLBoolean eglResult = ::eglInitialize(m_deviceContext.display, 0, 0);
			debug::Assert(eglResult, "eglInitialize failed.");

			//Create EGL surface
			EGLint configAttribs[] =
			{
				EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
				EGL_RED_SIZE, 8,
				EGL_GREEN_SIZE, 8,
				EGL_BLUE_SIZE, 8,
				EGL_ALPHA_SIZE, 8,
				EGL_NONE
			};

			EGLint numConfigs = 0;
			eglResult = ::eglChooseConfig(m_deviceContext.display, configAttribs, &m_deviceContext.config, 1, &numConfigs);
			debug::Assert(eglResult && numConfigs == 1, "eglChooseConfig failed.");

			EGLint format;
			eglGetConfigAttrib(m_deviceContext.display, m_deviceContext.config, EGL_NATIVE_VISUAL_ID, &format);
			ANativeWindow_setBuffersGeometry(m_windowHandle, 0, 0, format);

			m_deviceContext.surface = ::eglCreateWindowSurface(m_deviceContext.display, m_deviceContext.config, m_windowHandle, 0);
			debug::Assert(m_deviceContext.surface != EGL_NO_SURFACE, "eglCreateWindowSurface failed.");
		}

		WindowAndroid::~WindowAndroid()
		{
		}

		bool WindowAndroid::Update()
		{
			return true;
		}

		bool WindowAndroid::Resize(u32 clientAreaWidth, u32 clientAreaHeight, bool adjustForTitle)
		{
			//Set new client area size
			m_clientAreaWidth = clientAreaWidth;
			m_clientAreaHeight = clientAreaHeight;

			m_windowWidth = clientAreaWidth;
			m_windowHeight = clientAreaHeight;

			return true;
		}

		bool WindowAndroid::SetFullscreen(bool fullscreen, int displayIdx)
		{
			return true;
		}

		void WindowAndroid::SetTitle(const std::string& title)
		{

		}

		void WindowAndroid::ShowCursor(bool show)
		{

		}

		bool WindowAndroid::HasFocus() const
		{
			return true;
		}

		u32 WindowAndroid::GetDesktopWidth(int displayIdx) const
		{
			return 640;
		}

		u32 WindowAndroid::GetDesktopHeight(int displayIdx) const
		{
			return 480;
		}

		int WindowAndroid::GetDisplays(std::vector<Display>& displays) const
		{
			Display display;
			display.index = 0;
			display.name = "Main";
			display.size.x = m_windowWidth;
			display.size.y = m_windowHeight;
			displays.push_back(display);

			return displays.size();
		}

		int WindowAndroid::GetSupportedResolutions(std::vector<Vector2i>& resolutions, int displayIdx) const
		{
			// Android_TODO
			resolutions.push_back(Vector2i(1280, 720));
			return resolutions.size();
		}
	}
}