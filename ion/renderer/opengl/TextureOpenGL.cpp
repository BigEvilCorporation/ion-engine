///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		TextureOpenGL.cpp
// Date:		13th December 2013
// Authors:		Matt Phillips
// Description:	OpenGL texture implementation
///////////////////////////////////////////////////

#include "core/debug/Debug.h"
#include "core/memory/Memory.h"
#include "renderer/OpenGL/TextureOpenGL.h"
#include "renderer/OpenGL/RendererOpenGL.h"

#if defined ION_RENDER_SUPPORTS_SDL
#include <SDL/SDL.h>
#endif

#if defined ION_RENDER_SUPPORTS_SDLIMAGE
#include <SDLImage/SDL_image.h>
#endif

namespace ion
{
	namespace render
	{
		Texture* Texture::Create()
		{
			return new TextureOpenGL();
		}

		Texture* Texture::Create(u32 width, u32 height)
		{
			return new TextureOpenGL(width, height);
		}

		Texture* Texture::Create(u32 width, u32 height, Format sourceFormat, Format destFormat, BitsPerPixel bitsPerPixel, bool generateMipmaps, const u8* data)
		{
			return new TextureOpenGL(width, height, sourceFormat, destFormat, bitsPerPixel, generateMipmaps, data);
		}

		void Texture::RegisterSerialiseType(io::Archive& archive)
		{
			archive.RegisterPointerTypeStrict<Texture, TextureOpenGL>();
		}

		TextureOpenGL::TextureOpenGL()
		{
			m_glTextureId = 0;
			m_bitsPerPixel = eBPP8;

			//Generate texture
			glGenTextures(1, &m_glTextureId);
		}

		TextureOpenGL::TextureOpenGL(u32 width, u32 height)
			: Texture(width, height)
		{
			m_glTextureId = 0;
			m_bitsPerPixel = eBPP8;

			//Generate texture
			glGenTextures(1, &m_glTextureId);
		}

		TextureOpenGL::TextureOpenGL(u32 width, u32 height, Format sourceFormat, Format destFormat, BitsPerPixel bitsPerPixel,  bool generateMipmaps, const u8* data)
			: TextureOpenGL(width, height)
		{
			m_glTextureId = 0;
			m_bitsPerPixel = eBPP8;

			//Generate texture
			glGenTextures(1, &m_glTextureId);

			//Load from data
			Load(width, height, sourceFormat, destFormat, bitsPerPixel, generateMipmaps, data);
		}

		TextureOpenGL::~TextureOpenGL()
		{

		}

		bool TextureOpenGL::Load()
		{
#if defined ION_RENDER_SUPPORTS_SDLIMAGE
			//Load image onto a new SDL surface
			SDL_Surface* sdlSurface = IMG_Load(m_imageFilename.c_str());

			if(sdlSurface)
			{
				//Get dimensions
				m_width = sdlSurface->w;
				m_height = sdlSurface->h;

				Format format = eRGB;

				//Check colour format
				if(sdlSurface->format->BytesPerPixel == 4)
				{
					if (sdlSurface->format->Rmask == 0x000000ff)
						format = eRGBA;
					else
						format = eBGRA;
				}
				else if(sdlSurface->format->BytesPerPixel == 3)
				{
					if (sdlSurface->format->Rmask == 0x000000ff)
						format = eRGB;
					else
						format = eBGR;
				}
				
				//Load image to OpenGL texture
				Load(m_width, m_height, format, eRGBA_DXT5, BitsPerPixel(sdlSurface->format->BytesPerPixel * 8), true, (const u8*)sdlSurface->pixels);

				//Free SDL surface
				SDL_FreeSurface(sdlSurface);
			}
#endif

			return m_glTextureId != 0;
		}

		bool TextureOpenGL::Load(u32 width, u32 height, Format sourceFormat, Format destFormat, BitsPerPixel bitsPerPixel, bool generateMipmaps, const u8* data)
		{
			debug::Assert(data != NULL, "TextureOpenGL::Load() - NULL texture data");

			if(!m_glTextureId)
			{
				//Generate texture
				glGenTextures(1, &m_glTextureId);
			}

			debug::Assert(m_glTextureId != 0, "Could not create OpenGL texture");

			//Get GL format
			int glByteFormat = 0;
			GetOpenGLMode(destFormat, bitsPerPixel, m_glFormat, glByteFormat);
			m_bitsPerPixel = bitsPerPixel;

			//Bind the texture
			glBindTexture(GL_TEXTURE_2D, m_glTextureId);

			//Generate mipmaps
#if defined ION_RENDERER_KGL && defined ION_RENDER_SUPPORTS_GLUT
			if(generateMipmaps)
			{
				gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, m_glFormat, 0, data);
			}
#else
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, generateMipmaps ? GL_TRUE : GL_FALSE);
#endif

			//Set default filters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#if defined ION_RENDERER_KGL
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
#endif

			//Copy all pixels in surface to GL texture
			if(width == m_width && height == m_height)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, m_glFormat, width, height, 0, m_glFormat, glByteFormat, data);
			}
#if !defined ION_RENDERER_KGL
			else
			{
				glTexImage2D(GL_TEXTURE_2D, 0, m_glFormat, m_width, m_height, 0, m_glFormat, glByteFormat, NULL);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, m_glFormat, glByteFormat, data);
			}
#endif

			//Unbind texture
			glBindTexture(GL_TEXTURE_2D, 0);

			RendererOpenGL::CheckGLError();

			return m_glTextureId != 0;
		}

		void TextureOpenGL::Unload()
		{
			if(m_glTextureId)
			{
				glDeleteTextures(1, &m_glTextureId);
				m_glTextureId = 0;
			}
		}

		GLuint TextureOpenGL::GetTextureId() const
		{
			return m_glTextureId;
		}

		void TextureOpenGL::SetPixel(const ion::Vector2i& position, const Colour& colour)
		{
#if !defined ION_RENDERER_KGL
			u8 data[4];

			data[0] = (int)(colour.r * 255);
			data[1] = (int)(colour.g * 255);
			data[2] = (int)(colour.b * 255);
			data[3] = (int)(colour.a * 255);

			glBindTexture(GL_TEXTURE_2D, m_glTextureId);
			glTexSubImage2D(GL_TEXTURE_2D, 0, position.x, position.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glBindTexture(GL_TEXTURE_2D, 0);

			RendererOpenGL::CheckGLError();
#endif
		}

		void TextureOpenGL::SetPixels(Format sourceFormat, u8* data)
		{
#if !defined ION_RENDERER_KGL
			GLint glMode;
			GLint glByteFormat;
			GetOpenGLMode(sourceFormat, m_bitsPerPixel, glMode, glByteFormat);
			glBindTexture(GL_TEXTURE_2D, m_glTextureId);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, glMode, glByteFormat, data);
			glBindTexture(GL_TEXTURE_2D, 0);

			RendererOpenGL::CheckGLError();
#endif
		}

		void TextureOpenGL::GetPixels(const ion::Vector2i& position, const ion::Vector2i& size, Format format, BitsPerPixel bitsPerPixel, u8* data) const
		{
#if !defined ION_RENDERER_KGL
			const u32 bytesPerPixel = (u32)bitsPerPixel / 8;
			const u32 bufferSize = size.x * size.y * bytesPerPixel;
			u8* buffer = new u8[bufferSize];
			u8* dest = data;
			u32 lineSize = size.x * bytesPerPixel;

			GLint glMode;
			GLint glByteFormat;
			GetOpenGLMode(format, m_bitsPerPixel, glMode, glByteFormat);

			glBindTexture(GL_TEXTURE_2D, m_glTextureId);
			glGetTexImage(GL_TEXTURE_2D, 0, glMode, glByteFormat, buffer);
			glBindTexture(GL_TEXTURE_2D, 0);

			RendererOpenGL::CheckGLError();

			for(int y = position.y; y < size.y; y++)
			{
				u8* source = buffer + (((size.x * y) + position.x) * bytesPerPixel);
				
				memory::MemCopy(dest, source, lineSize);
				dest += lineSize;
			}

			delete buffer;
#endif
		}

		void TextureOpenGL::SetMinifyFilter(Filter filter)
		{
			glBindTexture(GL_TEXTURE_2D, m_glTextureId);

			int filterMode = 0;

			switch(filter)
			{
			case eFilterNearest:
				filterMode = GL_NEAREST;
				break;
			case eFilterLinear:
				filterMode = GL_LINEAR;
				break;
			case eFilterMipMapLinear:
				filterMode = GL_LINEAR_MIPMAP_LINEAR;
				break;
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);

			glBindTexture(GL_TEXTURE_2D, 0);

			RendererOpenGL::CheckGLError();
		}

		void TextureOpenGL::SetMagnifyFilter(Filter filter)
		{
			glBindTexture(GL_TEXTURE_2D, m_glTextureId);

			int filterMode = 0;

			switch(filter)
			{
			case eFilterNearest:
				filterMode = GL_NEAREST;
				break;
			case eFilterLinear:
				filterMode = GL_LINEAR;
				break;
			default:
				ion::debug::Error("Unsupported magnify filter mode");
				break;
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

			glBindTexture(GL_TEXTURE_2D, 0);

			RendererOpenGL::CheckGLError();
		}

		void TextureOpenGL::SetWrapping(Wrapping wrapping)
		{
			glBindTexture(GL_TEXTURE_2D, m_glTextureId);

			int wrapMode = 0;

			switch(wrapping)
			{
			case eWrapClamp:
				wrapMode = GL_CLAMP;
				break;
			case eWrapRepeat:
				wrapMode = GL_REPEAT;
				break;
#if !defined ION_RENDERER_KGL
			case eWrapMirror:
				wrapMode = GL_MIRRORED_REPEAT;
				break;
#endif
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

			glBindTexture(GL_TEXTURE_2D, 0);

			RendererOpenGL::CheckGLError();
		}

		void TextureOpenGL::GetOpenGLMode(Format format, BitsPerPixel bitsPerPixel, int& mode, int& byteFormat)
		{
			switch(bitsPerPixel)
			{
			case eBPP8:
				switch(format)
				{
#if !defined ION_RENDERER_KGL
				case eRGB:
					mode = GL_RGB8;
					byteFormat = GL_UNSIGNED_BYTE;
					break;
				case eRGBA:
					mode = GL_RGBA8;
					byteFormat = GL_UNSIGNED_BYTE;
					break;
#endif
				default:
					break;
				}
				break;
			case eBPP16:
				switch(format)
				{
				case eRGB:
					mode = GL_RGB;
					byteFormat = GL_UNSIGNED_SHORT_4_4_4_4;
					break;
				case eRGBA:
					mode = GL_RGBA;
					byteFormat = GL_UNSIGNED_SHORT_4_4_4_4;
					break;
				default:
					break;
				}
				break;
			case eBPP24:
				switch(format)
				{
				case eRGB:
					mode = GL_RGB;
					byteFormat = GL_UNSIGNED_BYTE;
					break;
				case eRGBA:
					mode = GL_RGBA;
					byteFormat = GL_UNSIGNED_BYTE;
					break;
#if !defined ION_RENDERER_KGL
				case eBGRA:
					mode = GL_BGRA;
					byteFormat = GL_UNSIGNED_BYTE;
					break;
				case eRGBA_DXT5:
					mode = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
					byteFormat = GL_UNSIGNED_BYTE;
					break;
#endif
				default:
					break;
				}
				break;
#if !defined ION_RENDERER_KGL
			case eBPP32:
				switch(format)
				{
				case eRGB:
					mode = GL_RGB32I;
					byteFormat = GL_UNSIGNED_BYTE;
					break;
				case eRGBA:
					mode = GL_RGBA32I;
					byteFormat = GL_UNSIGNED_BYTE;
					break;
				default:
					break;
				}
				break;
#endif
			default:
				ion::debug::Error("Unsupported texture data format");
				break;
			}
		}
	}
}