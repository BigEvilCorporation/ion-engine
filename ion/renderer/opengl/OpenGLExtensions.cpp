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

namespace ion
{
	namespace render
	{
#if defined ION_PLATFORM_WINDOWS
		PFNGLACTIVETEXTUREARBPROC OpenGLExt::glActiveTextureARB = NULL;
		PFNGLGENFRAMEBUFFERSEXTPROC OpenGLExt::glGenFramebuffersEXT = NULL;
		PFNGLBINDFRAMEBUFFEREXTPROC OpenGLExt::glBindFramebufferEXT = NULL;
		PFNGLGENRENDERBUFFERSEXTPROC OpenGLExt::glGenRenderbuffersEXT = NULL;
		PFNGLBINDRENDERBUFFEREXTPROC OpenGLExt::glBindRenderbufferEXT = NULL;
		PFNGLGENBUFFERSARBPROC OpenGLExt::glGenBuffersARB = NULL;
		PFNGLBINDBUFFERARBPROC OpenGLExt::glBindBufferARB = NULL;
		PFNGLDELETEBUFFERSARBPROC OpenGLExt::glDeleteBuffersARB = NULL;
		PFNGLBUFFERDATAARBPROC OpenGLExt::glBufferDataARB = NULL;
		PFNGLMAPBUFFERARBPROC OpenGLExt::glMapBufferARB = NULL;
		PFNGLUNMAPBUFFERARBPROC OpenGLExt::glUnmapBufferARB = NULL;
		PFNGLRENDERBUFFERSTORAGEEXTPROC OpenGLExt::glRenderbufferStorageEXT = NULL;
		PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC OpenGLExt::glFramebufferRenderbufferEXT = NULL;
		PFNGLFRAMEBUFFERTEXTURE2DEXTPROC OpenGLExt::glFramebufferTexture2DEXT = NULL;
		PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC OpenGLExt::glCheckFramebufferStatusEXT = NULL;
		PFNGLDELETEFRAMEBUFFERSEXTPROC OpenGLExt::glDeleteFramebuffersEXT = NULL;
		PFNGLDELETERENDERBUFFERSEXTPROC OpenGLExt::glDeleteRenderbuffersEXT = NULL;
		PFNGLDRAWBUFFERSPROC OpenGLExt::glDrawBuffers = NULL;
		PFNGLSWAPINTERVALEXTPROC OpenGLExt::glSwapIntervalEXT = NULL;
		DEBUGMESSAGECALLBACKPROC OpenGLExt::glDebugMessageCallback = NULL;
		PFNWGLCREATECONTEXTATTRIBSARBPROC OpenGLExt::wglCreateContextAttribsARB = NULL;
#endif

		void OpenGLExt::LoadExtensions(DeviceContext deviceContext)
		{
#if defined ION_PLATFORM_WINDOWS
			RenderContext context = wglCreateContext(deviceContext);
			wglMakeCurrent(deviceContext, context);
			glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
			glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
			glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
			glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
			glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
			glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
			glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
			glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
			glCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
			glDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
			glDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
			glDrawBuffers = (PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffers");
			glSwapIntervalEXT = (PFNGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
			glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffers");
			glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBuffer");
			glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffers");
			glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferData");
			glMapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBuffer");
			glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBuffer");
			wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
			glDebugMessageCallback = (DEBUGMESSAGECALLBACKPROC)wglGetProcAddress("glDebugMessageCallback");
			wglDeleteContext(context);
#endif
		}
	}
}
