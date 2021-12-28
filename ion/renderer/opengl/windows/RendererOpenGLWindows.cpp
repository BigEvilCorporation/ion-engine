// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		RendererOpenGL.cpp
// Date:		4th December 2013
// Authors:		Matt Phillips
// Description:	OpenGL renderer implementation
///////////////////////////////////////////////////

#include "core/debug/Debug.h"
#include "renderer/Colour.h"
#include "renderer/Viewport.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include "renderer/Shader.h"
#include "renderer/opengl/OpenGLVersion.h"
#include "renderer/opengl/RendererOpenGL.h"
#include "renderer/opengl/OpenGLExtensions.h"

//Force dual-GPU laptops to use dedicated GPU
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

namespace ion
{
	namespace render
	{

#if defined DEBUG

#define ION_RENDERER_LOG_WARNINGS 0
#define ION_RENDERER_WARNING_AS_ERROR 0

		void OnOpenGLError(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
		{
#if !ION_RENDERER_LOG_WARNINGS
			if (type == GL_DEBUG_TYPE_ERROR || type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
#endif
			{
#if !ION_RENDERER_WARNING_AS_ERROR
				if (type == GL_DEBUG_TYPE_ERROR || type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR)
#endif
				{
					debug::error
						<< "==============================================================\n"
						<< "OpenGL error: \n"
						<< "==============================================================\n"
						<< "GLenum source = " << source << "\n"
						<< "GLenum type = " << type << "\n"
						<< "GLuint id = " << id << "\n"
						<< "GLenum severity = " << severity << "\n"
						<< "GLsizei length = " << length << "\n"
						<< "const GLchar* message = " << message << "\n"
						<< "==============================================================\n"
						<< debug::end;
				}
#if !ION_RENDERER_WARNING_AS_ERROR
				else
				{
					debug::log
						<< "==============================================================\n"
						<< "OpenGL warning: \n"
						<< "==============================================================\n"
						<< "GLenum source = " << source << "\n"
						<< "GLenum type = " << type << "\n"
						<< "GLuint id = " << id << "\n"
						<< "GLenum severity = " << severity << "\n"
						<< "GLsizei length = " << length << "\n"
						<< "const GLchar* message = " << message << "\n"
						<< "==============================================================\n"
						<< debug::end;
				}
#endif
			}
		}
#endif

		void RendererOpenGL::InitPlatformGL(DeviceContext deviceContext)
		{
		}

		RenderContext RendererOpenGL::CreateContext(DeviceContext deviceContext, RenderContext sharedFrom)
		{
			//Create OpenGL context
			RenderContext renderContext;

			if (opengl::extensions->wglCreateContextAttribs)
			{
				int flags[] =
				{
					WGL_CONTEXT_MAJOR_VERSION_ARB, OPENGL_MAJOR_VERSION,
					WGL_CONTEXT_MINOR_VERSION_ARB, OPENGL_MINOR_VERSION,
					WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
	#if defined DEBUG
						WGL_CONTEXT_FLAGS_ARB,  WGL_CONTEXT_DEBUG_BIT_ARB,
	#endif
						0
				};

				renderContext = opengl::extensions->wglCreateContextAttribs(deviceContext, sharedFrom, flags);
			}
			else
			{
				renderContext = wglCreateContext(deviceContext);

				if (sharedFrom)
				{
					//Share with another context
					wglShareLists(sharedFrom, renderContext);
				}
			}

			if (!renderContext)
			{
				debug::Error("Could not create OpenGL context");
			}

#if defined DEBUG
			if (opengl::extensions->glDebugMessageCallback)
			{
				glEnable(GL_DEBUG_OUTPUT);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				opengl::extensions->glDebugMessageCallback(OnOpenGLError, nullptr);
			}
#endif

			return renderContext;
		}

		bool RendererOpenGL::SetThreadContext(DeviceContext deviceContext, RenderContext renderContext)
		{
			if (!wglMakeCurrent(deviceContext, renderContext))
			{
				ion::debug::error << "Error setting GL context: " << ion::platform::GetLastWin32Error() << ion::debug::end;
				return false;
			}

			return true;
		}

		void RendererOpenGL::SwapBuffers()
		{
			CheckGLContext();
			DeviceContext deviceContext = s_currentDC.GetData();
			wglSwapLayerBuffers(deviceContext, WGL_SWAP_MAIN_PLANE);
			CheckGLError("RendererOpenGL::SwapBuffers");
		}

		void RendererOpenGL::SetBlendColour(const Colour& colour)
		{
			if(opengl::extensions)
				opengl::extensions->glBlendColor(colour.r, colour.g, colour.b, colour.a);
		}

		void RendererOpenGL::LoadColourPalette(int paletteIdx, const std::vector<Colour>& palette)
		{
			ion::debug::Error("RendererOpenGL::LoadColourPalette() - Not implemented on platform");
		}

		void RendererOpenGL::EnableVSync(bool enabled)
		{
			if (opengl::extensions->glSwapInterval)
				opengl::extensions->glSwapInterval(enabled ? 1 : 0);
		}

		void RendererOpenGL::SetLineWidth(float width)
		{
			glLineWidth(width);
		}
	}
}