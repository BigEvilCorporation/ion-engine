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
		PFNGLACTIVETEXTUREARBPROC OpenGLExt::glActiveTexture = NULL;
		PFNGLGENFRAMEBUFFERSEXTPROC OpenGLExt::glGenFramebuffers = NULL;
		PFNGLBINDFRAMEBUFFEREXTPROC OpenGLExt::glBindFramebuffer = NULL;
		PFNGLGENRENDERBUFFERSEXTPROC OpenGLExt::glGenRenderbuffers = NULL;
		PFNGLBINDRENDERBUFFEREXTPROC OpenGLExt::glBindRenderbuffer = NULL;
		PFNGLGENBUFFERSARBPROC OpenGLExt::glGenBuffers = NULL;
		PFNGLBINDBUFFERARBPROC OpenGLExt::glBindBuffer = NULL;
		PFNGLDELETEBUFFERSARBPROC OpenGLExt::glDeleteBuffers = NULL;
		PFNGLBUFFERDATAARBPROC OpenGLExt::glBufferData = NULL;
		PFNGLMAPBUFFERARBPROC OpenGLExt::glMapBuffer = NULL;
		PFNGLMAPBUFFERRANGEPROC OpenGLExt::glMapBufferRange = NULL;
		PFNGLUNMAPBUFFERARBPROC OpenGLExt::glUnmapBuffer = NULL;
		PFNGLBUFFERSTORAGEPROC OpenGLExt::glBufferStorage = NULL;
		PFNGLRENDERBUFFERSTORAGEEXTPROC OpenGLExt::glRenderbufferStorage = NULL;
		PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC OpenGLExt::glFramebufferRenderbuffer = NULL;
		PFNGLFRAMEBUFFERTEXTURE2DEXTPROC OpenGLExt::glFramebufferTexture2D = NULL;
		PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC OpenGLExt::glCheckFramebufferStatus = NULL;
		PFNGLDELETEFRAMEBUFFERSEXTPROC OpenGLExt::glDeleteFramebuffers = NULL;
		PFNGLDELETERENDERBUFFERSEXTPROC OpenGLExt::glDeleteRenderbuffers = NULL;
		PFNGLDRAWBUFFERSPROC OpenGLExt::glDrawBuffers = NULL;
		PFNGLCLIENTWAITSYNCPROC OpenGLExt::glClientWaitSync = NULL;
		PFNGLFENCESYNCPROC OpenGLExt::glFenceSync = NULL;
		PFNGLDELETESYNCPROC OpenGLExt::glDeleteSync = NULL;
		PFNGLBLENDCOLOREXTPROC OpenGLExt::glBlendColor = NULL;
		PFNGLCOLORTABLEPROC OpenGLExt::glColorTable = NULL;
		PFNGLSWAPINTERVALEXTPROC OpenGLExt::glSwapInterval = NULL;
		DEBUGMESSAGECALLBACKPROC OpenGLExt::glDebugMessageCallback = NULL;
		PFNWGLCREATECONTEXTATTRIBSARBPROC OpenGLExt::wglCreateContextAttribs = NULL;
#endif

		void OpenGLExt::LoadExtensions(DeviceContext deviceContext)
		{
#if defined ION_PLATFORM_WINDOWS
			RenderContext context = wglCreateContext(deviceContext);
			wglMakeCurrent(deviceContext, context);
			glActiveTexture = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress("glActiveTextureARB");
			glGenFramebuffers = (PFNGLGENFRAMEBUFFERSEXTPROC)wglGetProcAddress("glGenFramebuffersEXT");
			glBindFramebuffer = (PFNGLBINDFRAMEBUFFEREXTPROC)wglGetProcAddress("glBindFramebufferEXT");
			glGenRenderbuffers = (PFNGLGENRENDERBUFFERSEXTPROC)wglGetProcAddress("glGenRenderbuffersEXT");
			glBindRenderbuffer = (PFNGLBINDRENDERBUFFEREXTPROC)wglGetProcAddress("glBindRenderbufferEXT");
			glRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEEXTPROC)wglGetProcAddress("glRenderbufferStorageEXT");
			glFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)wglGetProcAddress("glFramebufferRenderbufferEXT");
			glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)wglGetProcAddress("glFramebufferTexture2DEXT");
			glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)wglGetProcAddress("glCheckFramebufferStatusEXT");
			glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSEXTPROC)wglGetProcAddress("glDeleteFramebuffersEXT");
			glDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERSEXTPROC)wglGetProcAddress("glDeleteRenderbuffersEXT");
			glDrawBuffers = (PFNGLDRAWBUFFERSPROC)wglGetProcAddress("glDrawBuffers");
			glSwapInterval = (PFNGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
			glGenBuffers = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffers");
			glBindBuffer = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBuffer");
			glDeleteBuffers = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffers");
			glBufferData = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferData");
			glMapBuffer = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBuffer");
			glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)wglGetProcAddress("glMapBufferRange");
			glUnmapBuffer = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBuffer");
			glBufferStorage = (PFNGLBUFFERSTORAGEPROC)wglGetProcAddress("glBufferStorage");
			glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)wglGetProcAddress("glClientWaitSync");
			glFenceSync = (PFNGLFENCESYNCPROC)wglGetProcAddress("glFenceSync");
			glDeleteSync = (PFNGLDELETESYNCPROC)wglGetProcAddress("glDeleteSync");
			glBlendColor = (PFNGLBLENDCOLOREXTPROC)wglGetProcAddress("glBlendColor");
			glColorTable = (PFNGLCOLORTABLEPROC)wglGetProcAddress("glColorTableEXT");
			wglCreateContextAttribs = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
			glDebugMessageCallback = (DEBUGMESSAGECALLBACKPROC)wglGetProcAddress("glDebugMessageCallback");
			wglDeleteContext(context);
#endif
		}
	}
}
