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
#include "renderer/opengl/TextureOpenGL.h"
#include "renderer/opengl/RendererOpenGL.h"
#include "renderer/opengl/OpenGLExtensions.h"

#if defined ION_RENDER_SUPPORTS_SDL
#if defined ION_PLATFORM_MACOSX
#include <SDL.h>
#elif defined ION_PLATFORM_LINUX
#include <SDL2/SDL.h>
#elif defined ION_PLATFORM_RASPBERRYPI
#include <SDL2/SDL.h>
#else
#include <SDL2/SDL.h>
#endif
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

		Texture* Texture::Create(u32 width, u32 height, Format sourceFormat, Format destFormat, BitsPerPixel bitsPerPixel, bool generateMipmaps, bool generatePixelBuffer, const u8* data)
		{
			return new TextureOpenGL(width, height, sourceFormat, destFormat, bitsPerPixel, generateMipmaps, generatePixelBuffer, data);
		}

		void Texture::RegisterSerialiseType(io::Archive& archive)
		{
			archive.RegisterPointerTypeStrict<Texture, TextureOpenGL>();
		}

		TextureOpenGL::TextureOpenGL()
		{
			m_glTextureId = 0;
			m_glPixelBufferId = 0;
			m_bitsPerPixel = eBPP8;

			//Generate texture
			glGenTextures(1, &m_glTextureId);
		}

		TextureOpenGL::TextureOpenGL(u32 width, u32 height)
			: Texture(width, height)
		{
			m_glTextureId = 0;
			m_glPixelBufferId = 0;
			m_bitsPerPixel = eBPP8;

			//Generate texture
			glGenTextures(1, &m_glTextureId);
		}

		TextureOpenGL::TextureOpenGL(u32 width, u32 height, Format sourceFormat, Format destFormat, BitsPerPixel bitsPerPixel,  bool generateMipmaps, bool generatePixelBuffer, const u8* data)
			: TextureOpenGL(width, height)
		{
			m_glTextureId = 0;
			m_glPixelBufferId = 0;
			m_bitsPerPixel = eBPP8;

			//Generate texture
			glGenTextures(1, &m_glTextureId);

			if (generatePixelBuffer)
			{
#if defined ION_PLATFORM_WINDOWS
				//Generate pixel buffer
				debug::Assert(OpenGLExt::glGenBuffersARB, "Cannot create pixel buffer without glGenBuffersARB extension");
				OpenGLExt::glGenBuffersARB(1, &m_glPixelBufferId);
#endif
			}

			//Load from data
			Load(width, height, sourceFormat, destFormat, bitsPerPixel, generateMipmaps, generatePixelBuffer, data);
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
				Load(m_width, m_height, format, format, BitsPerPixel(sdlSurface->format->BytesPerPixel * 8), true, false, (const u8*)sdlSurface->pixels);

				//Free SDL surface
				SDL_FreeSurface(sdlSurface);
			}
#endif

			return m_glTextureId != 0;
		}

		bool TextureOpenGL::Load(const std::string& filename)
		{
			SetImageFilename(filename);
			return Load();
		}

		bool TextureOpenGL::Load(u32 width, u32 height, Format sourceFormat, Format destFormat, BitsPerPixel bitsPerPixel, bool generateMipmaps, bool generatePixelBuffer, const u8* data)
		{
			if(!m_glTextureId)
			{
				//Generate texture
				glGenTextures(1, &m_glTextureId);
				RendererOpenGL::CheckGLError("TextureOpenGL::Load");
			}

			debug::Assert(m_glTextureId != 0, "Could not create OpenGL texture");

			//Get GL format
			int glByteFormat = 0;
			int glColourFormat = 0;
			int pixelSize = 0;
			GetOpenGLMode(destFormat, bitsPerPixel, m_glFormat, glByteFormat, glColourFormat, pixelSize);
			m_bitsPerPixel = bitsPerPixel;

			//Bind the texture
			glBindTexture(GL_TEXTURE_2D, m_glTextureId);
			RendererOpenGL::CheckGLError("TextureOpenGL::Load");

			//Set pixel alignment
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			//Generate mipmaps
#if !defined ION_RENDERER_OPENGLES
#if defined ION_RENDERER_KGL && defined ION_RENDER_SUPPORTS_GLUT
			if(generateMipmaps)
			{
				gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, width, height, m_glFormat, 0, data);
			}
#else
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, generateMipmaps ? GL_TRUE : GL_FALSE);
#endif
#endif //ION_RENDERER_OPENGLES

			//Set default filters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#if defined ION_RENDERER_KGL
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
#endif

			RendererOpenGL::CheckGLError("TextureOpenGL::Load");

			//Set size
			m_width = width;
			m_height = height;

			//Copy all pixels in surface to GL texture
			glTexImage2D(GL_TEXTURE_2D, 0, glColourFormat, width, height, 0, m_glFormat, glByteFormat, data);
			RendererOpenGL::CheckGLError("TextureOpenGL::Load");

			if (m_glPixelBufferId)
			{
#if defined ION_PLATFORM_WINDOWS
				//Bind the pixel buffer
				OpenGLExt::glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, m_glPixelBufferId);

				//Create data container and map
				OpenGLExt::glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_EXT, width * height * pixelSize, NULL, GL_STREAM_DRAW_ARB);
				unsigned char* pixelBufferData = (unsigned char*)OpenGLExt::glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, GL_WRITE_ONLY);

				//Copy pixels
				if (data)
				{
					ion::memory::MemCopy(pixelBufferData, data, width * height * pixelSize);
				}

				//Unmap
				if (!OpenGLExt::glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT))
				{
					debug::error << "TextureOpenGL::Load() - Could not unmap PBO" << debug::end;
				}

				//Unbind
				OpenGLExt::glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, 0);
#endif
			}

			//Unbind texture
			glBindTexture(GL_TEXTURE_2D, 0);

			RendererOpenGL::CheckGLError("TextureOpenGL::Load");

			return m_glTextureId != 0;
		}

		void TextureOpenGL::Unload()
		{
			if(m_glTextureId)
			{
				glDeleteTextures(1, &m_glTextureId);
				m_glTextureId = 0;
			}

			if (m_glPixelBufferId)
			{
#if defined ION_PLATFORM_WINDOWS
				OpenGLExt::glDeleteBuffersARB(1, &m_glPixelBufferId);
				m_glPixelBufferId = 0;
#endif
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

			RendererOpenGL::CheckGLError("TextureOpenGL::SetPixel");
#endif
		}

		void TextureOpenGL::SetPixels(Format sourceFormat, u8* data)
		{
#if !defined ION_RENDERER_KGL
			GLint glMode;
			GLint glByteFormat;
			GLint glColourFormat;
			int pixelSize = 0;
			GetOpenGLMode(sourceFormat, m_bitsPerPixel, glMode, glByteFormat, glColourFormat, pixelSize);

			if (m_glPixelBufferId)
			{
#if defined ION_PLATFORM_WINDOWS
				//Bind pixel buffer
				OpenGLExt::glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, m_glPixelBufferId);
				RendererOpenGL::CheckGLError("TextureOpenGL::SetPixels");

				//Bind texture
				glBindTexture(GL_TEXTURE_2D, m_glTextureId);
				RendererOpenGL::CheckGLError("TextureOpenGL::SetPixels");

				//Get buffer data ptr
				unsigned char* pixelBufferData = (unsigned char*)OpenGLExt::glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, GL_WRITE_ONLY);
				RendererOpenGL::CheckGLError("TextureOpenGL::SetPixels");

				//Copy data
				ion::memory::MemCopy(pixelBufferData, data, m_width * m_height * pixelSize);

				//Unmap buffer
				if (!OpenGLExt::glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT))
				{
					debug::error << "TextureOpenGL::SetPixels() - Could not unmap PBO" << debug::end;
				}
				RendererOpenGL::CheckGLError("TextureOpenGL::SetPixels");

				//Copy to texture
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, glMode, glByteFormat, 0);
				RendererOpenGL::CheckGLError("TextureOpenGL::SetPixels");

				//Unbind texture
				glBindTexture(GL_TEXTURE_2D, 0);
				RendererOpenGL::CheckGLError("TextureOpenGL::SetPixels");

				//Unbind buffer
				OpenGLExt::glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, 0);
				RendererOpenGL::CheckGLError("TextureOpenGL::SetPixels");
#endif
			}
			else
			{
				//Copy direct to texture
				glBindTexture(GL_TEXTURE_2D, m_glTextureId);
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, glMode, glByteFormat, data);
				glBindTexture(GL_TEXTURE_2D, 0);
			}

			RendererOpenGL::CheckGLError("TextureOpenGL::SetPixels");
#endif
		}

		void TextureOpenGL::GetPixels(const ion::Vector2i& position, const ion::Vector2i& size, Format format, BitsPerPixel bitsPerPixel, u8* data) const
		{
#if !defined ION_RENDERER_KGL && !defined ION_RENDERER_OPENGLES
			const u32 bytesPerPixel = (u32)bitsPerPixel / 8;
			const u32 bufferSize = size.x * size.y * bytesPerPixel;
			u8* buffer = new u8[bufferSize];
			u8* dest = data;
			u32 lineSize = size.x * bytesPerPixel;

			GLint glMode;
			GLint glByteFormat;
			GLint glColourFormat;
			int pixelSize = 0;
			GetOpenGLMode(format, m_bitsPerPixel, glMode, glByteFormat, glColourFormat, pixelSize);

			glBindTexture(GL_TEXTURE_2D, m_glTextureId);
			glGetTexImage(GL_TEXTURE_2D, 0, glMode, glByteFormat, buffer);
			glBindTexture(GL_TEXTURE_2D, 0);

			RendererOpenGL::CheckGLError("TextureOpenGL::GetPixels");

			for(int y = position.y; y < size.y; y++)
			{
				u8* source = buffer + (((size.x * y) + position.x) * bytesPerPixel);
				
				memory::MemCopy(dest, source, lineSize);
				dest += lineSize;
			}

			delete [] buffer;
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

			RendererOpenGL::CheckGLError("TextureOpenGL::SetMinifyFilter");
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

			RendererOpenGL::CheckGLError("TextureOpenGL::SetMagnifyFilter");
		}

		void TextureOpenGL::SetWrapping(Wrapping wrapping)
		{
			glBindTexture(GL_TEXTURE_2D, m_glTextureId);

			int wrapMode = 0;

			switch(wrapping)
			{
			case eWrapClamp:
#if defined ION_RENDERER_OPENGLES
				wrapMode = GL_CLAMP_TO_EDGE;
#else
				wrapMode = GL_CLAMP;
#endif
				break;
			case eWrapRepeat:
#if defined ION_RENDERER_OPENGLES
				wrapMode = GL_MIRRORED_REPEAT;
#else
				wrapMode = GL_REPEAT;
#endif
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

			RendererOpenGL::CheckGLError("TextureOpenGL::SetWrapping");
		}

		void TextureOpenGL::GetOpenGLMode(Format format, BitsPerPixel bitsPerPixel, int& mode, int& byteFormat, int& colourFormat, int& pixelSize)
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
					colourFormat = GL_RGB;
					pixelSize = 1;
					break;
				case eRGBA:
					mode = GL_RGBA8;
					byteFormat = GL_UNSIGNED_BYTE;
					colourFormat = GL_RGB;
					pixelSize = 1;
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
					colourFormat = GL_RGB;
					pixelSize = 2;
					break;
				case eRGBA:
					mode = GL_RGBA;
					byteFormat = GL_UNSIGNED_SHORT_4_4_4_4;
					colourFormat = GL_RGBA;
					pixelSize = 2;
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
					colourFormat = GL_RGB;
					pixelSize = 3;
					break;
				case eRGBA:
					mode = GL_RGBA;
					byteFormat = GL_UNSIGNED_BYTE;
					colourFormat = GL_RGBA;
					pixelSize = 4;
					break;
#if !defined ION_RENDERER_KGL
				case eBGRA:
					mode = GL_BGRA;
					byteFormat = GL_UNSIGNED_BYTE;
					colourFormat = GL_RGBA;
					pixelSize = 4;
					break;
				case eRGBA_DXT5:
					mode = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
					byteFormat = GL_UNSIGNED_BYTE;
					colourFormat = GL_RGBA;
					pixelSize = 4;
					break;
#endif
				default:
					break;
				}
				break;
#if !defined ION_RENDERER_KGL && !defined ION_PLATFORM_MACOSX
			case eBPP32:
				switch(format)
				{
				case eRGB:
					mode = GL_RGB;
					byteFormat = GL_UNSIGNED_INT_10_10_10_2;
					colourFormat = GL_RGB;
					pixelSize = 4;
					break;
				case eRGBA:
					mode = GL_RGBA;
					byteFormat = GL_UNSIGNED_INT_8_8_8_8;
					colourFormat = GL_RGBA;
					pixelSize = 4;
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
