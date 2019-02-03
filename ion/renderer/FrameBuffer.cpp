#include "FrameBuffer.h"

namespace ion
{
	namespace render
	{
		FrameBuffer::FrameBuffer(int width, int height)
			: m_width(width)
			, m_height(height)
		{
			m_texture = Texture::Create(width, height, Texture::Format::RGB, Texture::Format::RGB, Texture::BitsPerPixel::BPP24, false, true, nullptr);
		}

		FrameBuffer::~FrameBuffer()
		{
			delete m_texture;
		}

		Texture& FrameBuffer::GetTexture()
		{
			return *m_texture;
		}
	}
}
