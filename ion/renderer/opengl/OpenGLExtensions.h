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

typedef void (APIENTRYP PFNGLBLENDCOLOREXTPROC) (GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);

typedef void (APIENTRYP PFNGLCOLORTABLEPROC) (GLenum  target, GLenum  internalFormat, GLsizei width, GLenum  format, GLenum  type, const GLvoid  *data);

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
			static PFNGLACTIVETEXTUREARBPROC glActiveTexture;

			static PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffers;
			static PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebuffer;

			static PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffers;
			static PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbuffer;
			static PFNGLDELETERENDERBUFFERSEXTPROC glDeleteRenderbuffers;
			static PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorage;

			static PFNGLGENBUFFERSARBPROC glGenBuffers;
			static PFNGLBINDBUFFERARBPROC glBindBuffer;
			static PFNGLDELETEBUFFERSARBPROC glDeleteBuffers;
			static PFNGLBUFFERDATAARBPROC glBufferData;
			static PFNGLMAPBUFFERARBPROC glMapBuffer;
			static PFNGLMAPBUFFERRANGEPROC glMapBufferRange;
			static PFNGLUNMAPBUFFERARBPROC glUnmapBuffer;
			static PFNGLBUFFERSTORAGEPROC glBufferStorage;

			static PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbuffer;
			static PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2D;
			static PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatus;
			static PFNGLDELETEFRAMEBUFFERSEXTPROC glDeleteFramebuffers;

			static PFNGLDRAWBUFFERSPROC glDrawBuffers;
			static PFNGLSWAPINTERVALEXTPROC glSwapInterval;

			static PFNGLCLIENTWAITSYNCPROC glClientWaitSync;
			static PFNGLFENCESYNCPROC glFenceSync;
			static PFNGLDELETESYNCPROC glDeleteSync;

			static PFNGLBLENDCOLOREXTPROC glBlendColor;

			static PFNGLCOLORTABLEPROC glColorTable;

			static PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribs;
			static DEBUGMESSAGECALLBACKPROC glDebugMessageCallback;
#endif
		};
	}
}
