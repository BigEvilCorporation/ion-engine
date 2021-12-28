#include "FrameBufferOpenGL.h"
#include "TextureOpenGL.h"
#include "RendererOpenGL.h"
#include "OpenGLExtensions.h"

namespace ion
{
	namespace render
	{
		FrameBuffer* FrameBuffer::Create(u32 width, u32 height)
		{
			return new FrameBufferOpenGL(width, height);
		}

		FrameBufferOpenGL::FrameBufferOpenGL(int width, int height)
			: FrameBuffer(width, height)
		{
#if defined ION_PLATFORM_WINDOWS
			//Generate GL framebuffer
			opengl::extensions->glGenFramebuffers(1, &m_glFrameBuffer);
			opengl::extensions->glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBuffer);

			//Bind texture to framebuffer
			GLuint textureId = ((TextureOpenGL*)m_texture)->GetTextureId();
			glBindTexture(GL_TEXTURE_2D, textureId);
			opengl::extensions->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
			glBindTexture(GL_TEXTURE_2D, 0);

			//Unbind framebuffer
			opengl::extensions->glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
		}

		FrameBufferOpenGL::~FrameBufferOpenGL()
		{

		}

		void FrameBufferOpenGL::Bind()
		{
#if defined ION_PLATFORM_WINDOWS
			opengl::extensions->glBindFramebuffer(GL_FRAMEBUFFER, m_glFrameBuffer);
#endif
		}

		void FrameBufferOpenGL::UnBind()
		{
#if defined ION_PLATFORM_WINDOWS
			opengl::extensions->glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif
		}

		void FrameBufferOpenGL::ReadPixels(int x, int y, int width, int height, Texture::Format format, Texture::BitsPerPixel bitsPerPixel, u8* data)
		{
#if defined ION_PLATFORM_WINDOWS
			GLint glMode;
			GLint glByteFormat;
			GLint glColourFormat;
			int pixelSize = 0;
			TextureOpenGL::GetOpenGLMode(format, bitsPerPixel, glMode, glByteFormat, glColourFormat, pixelSize);

			Bind();
			glReadPixels(x, y, width, height, glMode, glByteFormat, data);
			UnBind();
#endif
		}
	}
}