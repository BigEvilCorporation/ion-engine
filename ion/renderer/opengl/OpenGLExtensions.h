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

#include <functional>
#include <string>

#if defined ION_PLATFORM_WINDOWS
#define ION_GL_EXT(funcname, extname, returntype, args) \
	class GL_Extension_##funcname : public std::function<returntype args> \
	{ \
	public: \
		typedef returntype(*FUNC_DEF)args; \
		GL_Extension_##funcname() : std::function<returntype args>((FUNC_DEF)wglGetProcAddress(#extname)) {} \
	}; \
	GL_Extension_##funcname funcname
#elif defined ION_PLATFORM_SWITCH
#define ION_GL_EXT(funcname, extname, returntype, args) \
	class GL_Extension_##funcname : public std::function<returntype args> \
	{ \
	public: \
		typedef returntype(*FUNC_DEF)args; \
		GL_Extension_##funcname() : std::function<returntype args>((FUNC_DEF)::extname) \
		{ \
		} \
	}; \
	GL_Extension_##funcname funcname
#else
#define ION_GL_EXT(funcname, extname, returntype, args) \
	class GL_Extension_##funcname : public std::function<returntype args> \
	{ \
	public: \
		typedef returntype(*FUNC_DEF)args; \
		GL_Extension_##funcname() : std::function<returntype args>((FUNC_DEF)funcname) {}
	}; \
	GL_Extension_##funcname funcname
#endif

#if defined ION_PLATFORM_WINDOWS || defined ION_PLATFORM_SWITCH
typedef void (*DEBUGPROC)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void *userParam);
#endif

namespace ion
{
	namespace render
	{
		class OpenGLExtensions
		{
		public:
			ION_GL_EXT(glActiveTexture, glActiveTextureARB, void, (GLenum texture));
			ION_GL_EXT(glGenFramebuffers, glGenFramebuffersEXT, void, (GLsizei n, GLuint *framebuffers));
			ION_GL_EXT(glBindFramebuffer, glBindFramebufferEXT, void, (GLenum target, GLuint framebuffer));
			ION_GL_EXT(glGenRenderbuffers, glGenRenderbuffersEXT, void, (GLsizei n, GLuint *renderbuffers));
			ION_GL_EXT(glBindRenderbuffer, glBindRenderbufferEXT, void, (GLenum target, GLuint renderbuffer));
			ION_GL_EXT(glRenderbufferStorage, glRenderbufferStorageEXT, void, (GLenum target, GLenum internalformat, GLsizei width, GLsizei height));
			ION_GL_EXT(glFramebufferRenderbuffer, glFramebufferRenderbufferEXT, void, (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer));
			ION_GL_EXT(glFramebufferTexture2D, glFramebufferTexture2DEXT, void, (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level));
			ION_GL_EXT(glCheckFramebufferStatus, glCheckFramebufferStatusEXT, GLenum, (GLenum target));
			ION_GL_EXT(glDeleteFramebuffers, glDeleteFramebuffersEXT, void, (GLsizei n, const GLuint *framebuffers));
			ION_GL_EXT(glDeleteRenderbuffers, glDeleteRenderbuffersEXT, void, (GLsizei n, const GLuint *renderbuffers));
			ION_GL_EXT(glDrawBuffers, glDrawBuffers, void, (GLsizei n, const GLenum *bufs));
			ION_GL_EXT(glGenBuffers, glGenBuffers, void, (GLsizei n, GLuint *buffers));
			ION_GL_EXT(glBindBuffer, glBindBuffer, void, (GLenum target, GLuint buffer));
			ION_GL_EXT(glDeleteBuffers, glDeleteBuffers, void, (GLsizei n, const GLuint *buffers));
			ION_GL_EXT(glBufferData, glBufferData, void, (GLenum target, GLsizeiptr size, const void *data, GLenum usage));
			ION_GL_EXT(glBufferSubData, glBufferSubData, void, (GLenum target, GLintptr offset, GLsizeiptr size, const void* data));
			ION_GL_EXT(glMapBuffer, glMapBuffer, void*, (GLenum target, GLenum access));
			ION_GL_EXT(glMapBufferRange, glMapBufferRange, void*, (GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access));
			ION_GL_EXT(glUnmapBuffer, glUnmapBuffer, GLboolean, (GLenum target));
			ION_GL_EXT(glBufferStorage, glBufferStorage, void, (GLenum target, GLsizeiptr size, const void *data, GLbitfield flags));
			ION_GL_EXT(glBlendColor, glBlendColor, void, (GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha));
			ION_GL_EXT(glColorTable, glColorTableEXT, void, (GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const void *table));

#if defined ION_GL_SUPPORT_FENCE
			ION_GL_EXT(glClientWaitSync, glClientWaitSync, GLenum, (GLsync sync, GLbitfield flags, GLuint64 timeout));
			ION_GL_EXT(glFenceSync, glFenceSync, GLsync, (GLenum condition, GLbitfield flags));
			ION_GL_EXT(glDeleteSync, glDeleteSync, void, (GLsync sync));
#endif

#if defined ION_PLATFORM_WINDOWS || defined ION_PLATFORM_SWITCH
			ION_GL_EXT(glDebugMessageCallback, glDebugMessageCallback, void, (DEBUGPROC callback, const void * userParam));
#endif

#if defined ION_PLATFORM_WINDOWS
			ION_GL_EXT(glGenerateMipmap, glGenerateMipmapARB, void, (GLenum texture));
			ION_GL_EXT(wglCreateContextAttribs, wglCreateContextAttribsARB, HGLRC, (HDC hDC, HGLRC hShareContext, const int *attribList));
			ION_GL_EXT(glSwapInterval, wglSwapIntervalEXT, void, (int interval));
#endif

#if defined ION_PLATFORM_SWITCH
			ION_GL_EXT(glGenerateMipmap, glGenerateMipmap, void, (GLenum texture));
#endif

#if defined ION_SHADER_LANG_GLSL
			ION_GL_EXT(glAttachObject, glAttachObjectARB, void, (GLuint containerObj, GLuint obj));
			ION_GL_EXT(glDeleteObject, glDeleteObjectARB, void, (GLuint obj));
			ION_GL_EXT(glCreateProgram, glCreateProgramObjectARB, GLuint, (void));
			ION_GL_EXT(glCompileShader, glCompileShader, void, (GLuint shaderObj));
			ION_GL_EXT(glCreateShader, glCreateShaderObjectARB, GLuint, (GLenum shaderType));
			ION_GL_EXT(glValidateProgram, glValidateProgram, void, (GLuint programObj));
			ION_GL_EXT(glLinkProgram, glLinkProgram, void, (GLuint programObj));
			ION_GL_EXT(glUseProgram, glUseProgram, void, (GLuint program));
			ION_GL_EXT(glShaderSource, glShaderSource, void, (GLuint shaderObj, GLsizei count, const GLchar **string, const GLint *length));
			ION_GL_EXT(glGetUniformLocation, glGetUniformLocation, GLint, (GLuint programObj, const GLchar *name));
			ION_GL_EXT(glGetProgramiv, glGetProgramiv, void, (GLuint program, GLenum pname, GLint *params));
			ION_GL_EXT(glGetShaderiv, glGetShaderiv, void, (GLuint shader, GLenum pname, GLint *params));
			ION_GL_EXT(glGetProgramInfoLog, glGetProgramInfoLog, void, (GLuint program, GLsizei maxLength, GLsizei *length, GLchar *infoLog));
			ION_GL_EXT(glGetShaderInfoLog, glGetShaderInfoLog, void, (GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog));

			ION_GL_EXT(glUniformMatrix4fv, glUniformMatrix4fv, void, (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value));
			ION_GL_EXT(glUniform1i, glUniform1i, void, (GLint location, GLint v0));
			ION_GL_EXT(glUniform1f, glUniform1f, void, (GLint location, GLfloat v0));
			ION_GL_EXT(glUniform2f, glUniform2f, void, (GLint location, GLfloat v0, GLfloat v1));
			ION_GL_EXT(glUniform3f, glUniform3f, void, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2));
			ION_GL_EXT(glUniform4f, glUniform4f, void, (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3));
			ION_GL_EXT(glUniform1fv, glUniform1fv, void, (GLint location, GLsizei count, const GLfloat* value));
			ION_GL_EXT(glUniform4fv, glUniform4fv, void, (GLint location, GLsizei count, const GLfloat* value));
#endif

			ION_GL_EXT(glVertexAttribPointer, glVertexAttribPointer, void, (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer));
			ION_GL_EXT(glEnableVertexAttribArray, glEnableVertexAttribArray, void, (GLuint index));
			ION_GL_EXT(glDisableVertexAttribArray, glDisableVertexAttribArray, void, (GLuint index));
			ION_GL_EXT(glBindVertexArray, glBindVertexArray, void, (GLuint));
			ION_GL_EXT(glGenVertexArrays, glGenVertexArrays, void, (GLsizei n, GLuint *arrays));
			ION_GL_EXT(glDeleteVertexArrays, glDeleteVertexArrays, void, (GLsizei n, const GLuint *arrays));
		};

		namespace opengl
		{
			void LoadExtensions(DeviceContext deviceContext);
			extern OpenGLExtensions* extensions;
		}
	}
}
