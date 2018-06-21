///////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		OpenGLExtensions.h
// Date:		13th June 2018
// Authors:		Matt Phillips
// Description:	OpenGL extensions
///////////////////////////////////////////////////

#pragma once

#include "core/Platform.h"
#include "renderer/Types.h"
#include "renderer/opengl/OpenGLInclude.h"

#if defined ION_PLATFORM_WINDOWS
typedef void (APIENTRYP PFNGLSWAPINTERVALEXTPROC) (int interval);
typedef void (APIENTRYP PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRYP PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRYP PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRYP PFNGLBUFFERDATAARBPROC) (GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage);
typedef GLvoid* (APIENTRYP PFNGLMAPBUFFERARBPROC) (GLenum target, GLenum access);
typedef GLboolean(APIENTRYP PFNGLUNMAPBUFFERARBPROC) (GLenum target);

typedef HGLRC(APIENTRYP PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);

typedef void (APIENTRYP DEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void *userParam);
typedef void (APIENTRYP DEBUGMESSAGECALLBACKPROC) (DEBUGPROC callback, const void * userParam);
#endif

namespace ion
{
	namespace render
	{
		class OpenGLExt
		{
		public:
			static void LoadExtensions(DeviceContext deviceContext);

#if defined ION_PLATFORM_WINDOWS
			static PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;

			static PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
			static PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;

			static PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
			static PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;
			static PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffersEXT;
			static PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;

			static PFNGLGENBUFFERSARBPROC glGenBuffersARB;
			static PFNGLBINDBUFFERARBPROC glBindBufferARB;
			static PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
			static PFNGLBUFFERDATAARBPROC glBufferDataARB;
			static PFNGLMAPBUFFERARBPROC glMapBufferARB;
			static PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;

			static PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
			static PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
			static PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatusEXT;
			static PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffersEXT;

			static PFNGLDRAWBUFFERSPROC glDrawBuffers;
			static PFNGLSWAPINTERVALEXTPROC glSwapIntervalEXT;

			static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
			static DEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
#endif
		};
	}
}
