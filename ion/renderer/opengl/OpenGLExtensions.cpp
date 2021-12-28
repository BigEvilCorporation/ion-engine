///////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		OpenGLExtensions.cpp
// Date:		13th June 2018
// Authors:		Matt Phillips
// Description:	OpenGL extensions
///////////////////////////////////////////////////

#include "core/debug/Debug.h"
#include "OpenGLExtensions.h"

#define ION_GL_EXT_LIST_EXTENSIONS 0

namespace ion
{
	namespace render
	{
		namespace opengl
		{
			void LoadExtensions(DeviceContext deviceContext)
			{
#if defined ION_PLATFORM_WINDOWS
				RenderContext context = wglCreateContext(deviceContext);
				wglMakeCurrent(deviceContext, context);
#endif

#if ION_GL_EXT_LIST_EXTENSIONS
				GLint n = 0;
				glGetIntegerv(GL_NUM_EXTENSIONS, &n);
				PFNGLGETSTRINGIPROC glGetStringi = (PFNGLGETSTRINGIPROC)wglGetProcAddress("glGetStringi");

				for (GLint i = 0; i < n; i++)
				{
					const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
					debug::log << "OpenGL extension " << i << ": " << extension << debug::end;
				}
#endif

				extensions = new OpenGLExtensions();

#if defined ION_PLATFORM_WINDOWS
				wglDeleteContext(context);
#endif
			}

			OpenGLExtensions* extensions = nullptr;
		}
	}
}
