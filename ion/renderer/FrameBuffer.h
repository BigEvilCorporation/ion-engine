#pragma once

#include "Texture.h"

namespace ion
{
	namespace render
	{
		class FrameBuffer
		{
		public:
			static FrameBuffer* Create(u32 width, u32 height);

			virtual ~FrameBuffer();

			virtual void Bind() = 0;
			virtual void UnBind() = 0;

			virtual void ReadPixels(int x, int y, int width, int height, Texture::Format format, Texture::BitsPerPixel bitsPerPixel, u8* data) = 0;

			int GetWidth() const { return m_width; }
			int GetHeight() const { return m_height; }

			Texture& GetTexture();

		protected:
			FrameBuffer(int width, int height);
			Texture* m_texture;
			int m_width;
			int m_height;
		};
	}
}