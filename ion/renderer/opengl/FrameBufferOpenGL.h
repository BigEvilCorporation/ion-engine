#pragma once

#include <ion/renderer/FrameBuffer.h>

#include "OpenGLInclude.h"

namespace ion
{
	namespace render
	{
		class FrameBufferOpenGL : public FrameBuffer
		{
		public:
			FrameBufferOpenGL(int width, int height);
			virtual ~FrameBufferOpenGL();

			virtual void Bind();
			virtual void UnBind();

			virtual void ReadPixels(int x, int y, int width, int height, Texture::Format format, Texture::BitsPerPixel bitsPerPixel, u8* data);

		private:
			GLuint m_glFrameBuffer;
		};
	}
}