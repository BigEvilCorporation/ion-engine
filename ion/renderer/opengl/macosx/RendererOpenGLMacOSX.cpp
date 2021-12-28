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

namespace ion
{
	namespace render
	{
		void RendererOpenGL::InitPlatformGL(DeviceContext deviceContext)
		{

		}

		RenderContext RendererOpenGL::CreateContext(DeviceContext deviceContext, RenderContext sharedFrom)
		{
			if (sharedFrom)
			{
				LockGLContext();
				SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
			}

			RenderContext renderContext = SDL_GL_CreateContext(deviceContext);

			if (sharedFrom)
			{
				UnlockGLContext();
			}

			return renderContext;
		}

		bool RendererOpenGL::SetThreadContext(DeviceContext deviceContext, RenderContext renderContext)
		{
			if (!SDL_GL_MakeCurrent(deviceContext, renderContext))
			{
				ion::debug::Error("Error setting GL context");
				return false;
			}

			return true;
		}

		void RendererOpenGL::SwapBuffers()
		{
			CheckGLContext();
			DeviceContext deviceContext = s_currentDC.GetData();
			SDL_GL_SwapWindow(deviceContext);
			CheckGLError("RendererOpenGL::SwapBuffers");
		}

		void RendererOpenGL::SetBlendColour(const Colour& colour)
		{
			ion::debug::Error("RendererOpenGL::SetBlendColour() - Not implemented on platform");
		}

		void RendererOpenGL::LoadColourPalette(int paletteIdx, const std::vector<Colour>& palette)
		{
			ion::debug::Error("RendererOpenGL::LoadColourPalette() - Not implemented on platform");
		}

		void RendererOpenGL::EnableVSync(bool enabled)
		{
			ion::debug::Error("RendererOpenGL::EnableVSync() - Not implemented on platform");
		}

		void RendererOpenGL::SetLineWidth(float width)
		{
			glLineWidth(width);
		}
	}
}