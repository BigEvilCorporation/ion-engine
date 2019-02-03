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
#include "renderer/sdl/SDLInclude.h"

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
			OpenGLContextStackLock lock;

#if defined ION_PLATFORM_WINDOWS
			ion::debug::Assert(wglGetCurrentContext(), "TextureOpenGL::TextureOpenGL() - No valid OpenGL context");
#endif

			m_glTextureId = 0;
			m_glPixelBufferId = 0;
			m_bitsPerPixel = BitsPerPixel::BPP8;

#if defined ION_GL_SUPPORT_FENCE
			m_persistentBuffer = nullptr;
#endif

			//Generate texture
			glGenTextures(1, &m_glTextureId);
		}

		TextureOpenGL::TextureOpenGL(u32 width, u32 height)
			: Texture(width, height)
		{
			OpenGLContextStackLock lock;

#if defined ION_PLATFORM_WINDOWS
			ion::debug::Assert(wglGetCurrentContext(), "TextureOpenGL::TextureOpenGL() - No valid OpenGL context");
#endif

			m_glTextureId = 0;
			m_glPixelBufferId = 0;
			m_paletteIdx = 0;
			m_bitsPerPixel = BitsPerPixel::BPP8;

#if defined ION_GL_SUPPORT_FENCE
			m_persistentBuffer = nullptr;
#endif

			//Generate texture
			glGenTextures(1, &m_glTextureId);
		}

		TextureOpenGL::TextureOpenGL(u32 width, u32 height, Format sourceFormat, Format destFormat, BitsPerPixel bitsPerPixel,  bool generateMipmaps, bool generatePixelBuffer, const u8* data)
			: TextureOpenGL(width, height)
		{
			OpenGLContextStackLock lock;

#if defined ION_PLATFORM_WINDOWS
			ion::debug::Assert(wglGetCurrentContext(), "TextureOpenGL::TextureOpenGL() - No valid OpenGL context");
#endif

			m_glTextureId = 0;
			m_glPixelBufferId = 0;
			m_paletteIdx = 0;
			m_bitsPerPixel = BitsPerPixel::BPP8;

#if defined ION_GL_SUPPORT_FENCE
			m_persistentBuffer = nullptr;
#endif

			//Generate texture
			glGenTextures(1, &m_glTextureId);

			if (generatePixelBuffer)
			{
#if defined ION_PLATFORM_WINDOWS
				//If supported
				const bool pboSupported =	(OpenGLExt::glMapBufferRange != nullptr)
										&& (OpenGLExt::glGenBuffers != nullptr)
										&& (OpenGLExt::glBufferStorage != nullptr)
										&& (OpenGLExt::glBindBuffer != nullptr);

				if (pboSupported)
				{
					//Generate pixel buffer
					debug::Assert(OpenGLExt::glGenBuffers, "Cannot create pixel buffer without glGenBuffersARB extension");
					OpenGLExt::glGenBuffers(1, &m_glPixelBufferId);
				}
#endif
			}

			//Load from data
			Load(width, height, sourceFormat, destFormat, bitsPerPixel, generateMipmaps, generatePixelBuffer, data);
		}

		TextureOpenGL::~TextureOpenGL()
		{
			Unload();
		}

		bool TextureOpenGL::Load()
		{
			OpenGLContextStackLock lock;

#if defined ION_RENDER_SUPPORTS_SDL2IMAGE
			//Load image onto a new SDL surface
			SDL_Surface* sdlSurface = IMG_Load(m_imageFilename.c_str());

			if(!sdlSurface)
			{
				debug::log << "TextureOpenGL::Load() - Error loading " << m_imageFilename.c_str() << ", IMG_Load() failed with: " << IMG_GetError() << debug::end;
			}

			if(sdlSurface)
			{
				//Get dimensions
				m_width = sdlSurface->w;
				m_height = sdlSurface->h;

				//Convert to RGBA
                SDL_PixelFormat format;
                format.palette = 0;
                format.BitsPerPixel = 32;
                format.BytesPerPixel = 4;
                
#if defined ION_ENDIAN_LITTLE
                format.Rmask = 0xFF000000; format.Rshift =  0; format.Rloss = 0;
                format.Gmask = 0x00FF0000; format.Gshift =  8; format.Gloss = 0;
                format.Bmask = 0x0000FF00; format.Bshift = 16; format.Bloss = 0;
                format.Amask = 0x000000FF; format.Ashift = 24; format.Aloss = 0;
#else
                format.Rmask = 0x000000FF; format.Rshift = 24; format.Rloss = 0;
                format.Gmask = 0x0000FF00; format.Gshift = 16; format.Gloss = 0;
                format.Bmask = 0x00FF0000; format.Bshift =  8; format.Bloss = 0;
                format.Amask = 0xFF000000; format.Ashift =  0; format.Aloss = 0;
#endif
				
                SDL_Surface* surfaceRGBA = SDL_ConvertSurface(sdlSurface, &format, SDL_SWSURFACE);
                
				//Load image to OpenGL texture
				Load(m_width, m_height, Format::RGBA, Format::RGBA, BitsPerPixel(surfaceRGBA->format->BytesPerPixel * 8), true, false, (const u8*)surfaceRGBA->pixels);

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
			OpenGLContextStackLock lock;

			if(!m_glTextureId)
			{
				//Generate texture
				glGenTextures(1, &m_glTextureId);
			}

			debug::Assert(m_glTextureId != 0, "Could not create OpenGL texture");

			//Get GL format
			int glByteFormat = 0;
			int glColourFormat = 0;
			int pixelSize = 0;
			GetOpenGLMode(destFormat, bitsPerPixel, m_glFormat, glByteFormat, glColourFormat, pixelSize);
			m_bitsPerPixel = bitsPerPixel;
			m_pixelSize = pixelSize;

			//Bind the texture
			glBindTexture(GL_TEXTURE_2D, m_glTextureId);

			//Set pixel alignment
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			//Convert data if necessary
			bool converted = false;

#if defined ION_RENDERER_KGL
			if (destFormat == ion::render::Texture::Format::RGBA_Indexed)
			{
				u8* twiddled = new u8[ion::maths::Square(ion::maths::Max(width, height))];
				u8* temp = twiddled;
				ConvertTwiddled(data, temp, width, 0, 0, width);
				data = twiddled;
			}
#endif

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
				OpenGLExt::glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, m_glPixelBufferId);

				//Create data container
				OpenGLExt::glBufferStorage(GL_PIXEL_UNPACK_BUFFER_EXT, width * height * pixelSize, 0, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

				//Map buffer persistently
				m_persistentBuffer = (u8*)OpenGLExt::glMapBufferRange(GL_PIXEL_UNPACK_BUFFER_EXT, 0, width * height * pixelSize, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

				//Copy pixels
				if (data)
				{
					ion::memory::MemCopy(m_persistentBuffer, data, width * height * pixelSize);
				}

				//Unbind pixel buffer
				OpenGLExt::glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, 0);

				//Create GPU sync fence
				m_glSyncObject = CreateFence();
#endif
			}

			//Unbind texture
			glBindTexture(GL_TEXTURE_2D, 0);

			//Delete conversion buffer
			if (converted)
			{
				delete data;
			}

			//Update stats
			s_textureMemoryUsed += (m_width * m_height * m_pixelSize);

			RendererOpenGL::CheckGLError("TextureOpenGL::Load");

			return m_glTextureId != 0;
		}

		void TextureOpenGL::Unload()
		{
			OpenGLContextStackLock lock;

			if(m_glTextureId)
			{
				glDeleteTextures(1, &m_glTextureId);
				m_glTextureId = 0;

				//Update stats
				s_textureMemoryUsed -= (m_width * m_height * m_pixelSize);
			}

			if (m_glPixelBufferId)
			{
#if defined ION_PLATFORM_WINDOWS
				//Destroy fence
				DestroyFence(m_glSyncObject);

				//Unmap
#if 0
				if (!OpenGLExt::glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_EXT))
				{
					debug::error << "TextureOpenGL::Unload() - Could not unmap PBO" << debug::end;
				}
#endif

				OpenGLExt::glDeleteBuffers(1, &m_glPixelBufferId);
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
			OpenGLContextStackLock lock;

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

		void TextureOpenGL::SetPixels(Format sourceFormat, bool synchronised, u8* data)
		{
			OpenGLContextStackLock lock;

#if !defined ION_RENDERER_KGL
			GLint glMode;
			GLint glByteFormat;
			GLint glColourFormat;
			int pixelSize = 0;
			GetOpenGLMode(sourceFormat, m_bitsPerPixel, glMode, glByteFormat, glColourFormat, pixelSize);
			int textureSizeBytes = m_width * m_height * pixelSize;

			if (m_glPixelBufferId)
			{
#if defined ION_PLATFORM_WINDOWS
				//Bind pixel buffer
				OpenGLExt::glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, m_glPixelBufferId);

				//Bind texture
				glBindTexture(GL_TEXTURE_2D, m_glTextureId);

				//Get buffer data ptr
				u8* pixelBufferData = nullptr;
				if (synchronised)
				{
					pixelBufferData = (unsigned char*)OpenGLExt::glMapBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, GL_WRITE_ONLY);
				}
				else
				{
					//Synchronise
					WaitFence(m_glSyncObject);
				}

				//Copy data
				ion::memory::MemCopy(m_persistentBuffer, data, m_width * m_height * pixelSize);

				//Copy to texture
				glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, glMode, glByteFormat, 0);

				//Unbind texture
				glBindTexture(GL_TEXTURE_2D, 0);

				//Unbind buffer
				OpenGLExt::glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, 0);
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
			OpenGLContextStackLock lock;

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

		u8* TextureOpenGL::LockPixelBuffer()
		{
			OpenGLContextStackLock lock;

			ion::debug::Assert(m_glPixelBufferId, "TextureOpenGL::LockPixelBuffer() - Not a PBO");

#if defined ION_PLATFORM_WINDOWS
			if (m_persistentBuffer)
			{
				GLint glMode;
				GLint glByteFormat;
				GLint glColourFormat;
				int pixelSize = 0;
				GetOpenGLMode(Format::RGB, m_bitsPerPixel, glMode, glByteFormat, glColourFormat, pixelSize);

				//Bind pixel buffer
				OpenGLExt::glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, m_glPixelBufferId);

				//Bind texture
				glBindTexture(GL_TEXTURE_2D, m_glTextureId);

				//Copy texture to buffer
				glGetTexImage(GL_TEXTURE_2D, 0, glMode, glByteFormat, m_persistentBuffer);

				//Unbind texture
				glBindTexture(GL_TEXTURE_2D, 0);

				//Unbind buffer
				OpenGLExt::glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, 0);

				return m_persistentBuffer;
			}
			else
			{
				//Get buffer data ptr
				return (u8*)OpenGLExt::glMapBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, GL_READ_WRITE);
			}
#else
			return nullptr;
#endif
		}

		void TextureOpenGL::UnlockPixelBuffer()
		{
			OpenGLContextStackLock lock;

			ion::debug::Assert(m_glPixelBufferId, "TextureOpenGL::UnlockPixelBuffer() - Not a PBO");

#if defined ION_PLATFORM_WINDOWS
			GLint glMode;
			GLint glByteFormat;
			GLint glColourFormat;
			int pixelSize = 0;
			GetOpenGLMode(Format::RGB, m_bitsPerPixel, glMode, glByteFormat, glColourFormat, pixelSize);

			//Bind pixel buffer
			OpenGLExt::glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, m_glPixelBufferId);

			//Bind texture
			glBindTexture(GL_TEXTURE_2D, m_glTextureId);

			//Copy buffer to texture
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, glMode, glByteFormat, 0);

			//Unbind texture
			glBindTexture(GL_TEXTURE_2D, 0);

			//Unbind buffer
			OpenGLExt::glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, 0);
#endif
		}

		void TextureOpenGL::SetMinifyFilter(Filter filter)
		{
			OpenGLContextStackLock lock;

			glBindTexture(GL_TEXTURE_2D, m_glTextureId);

			int filterMode = 0;

			switch(filter)
			{
			case Filter::Nearest:
				filterMode = GL_NEAREST;
				break;
			case Filter::Linear:
				filterMode = GL_LINEAR;
				break;
			case Filter::MipMapLinear:
				filterMode = GL_LINEAR_MIPMAP_LINEAR;
				break;
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);

			glBindTexture(GL_TEXTURE_2D, 0);

			RendererOpenGL::CheckGLError("TextureOpenGL::SetMinifyFilter");
		}

		void TextureOpenGL::SetMagnifyFilter(Filter filter)
		{
			OpenGLContextStackLock lock;

			glBindTexture(GL_TEXTURE_2D, m_glTextureId);

			int filterMode = 0;

			switch(filter)
			{
			case Filter::Nearest:
				filterMode = GL_NEAREST;
				break;
			case Filter::Linear:
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
			OpenGLContextStackLock lock;

			glBindTexture(GL_TEXTURE_2D, m_glTextureId);

			int wrapMode = 0;

			switch(wrapping)
			{
			case Wrapping::Clamp:
#if defined ION_RENDERER_OPENGLES
				wrapMode = GL_CLAMP_TO_EDGE;
#else
				wrapMode = GL_CLAMP;
#endif
				break;
			case Wrapping::Repeat:
#if defined ION_RENDERER_OPENGLES
				wrapMode = GL_MIRRORED_REPEAT;
#else
				wrapMode = GL_REPEAT;
#endif
				break;
#if !defined ION_RENDERER_KGL
			case Wrapping::Mirror:
				wrapMode = GL_MIRRORED_REPEAT;
				break;
#endif
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

			glBindTexture(GL_TEXTURE_2D, 0);

			RendererOpenGL::CheckGLError("TextureOpenGL::SetWrapping");
		}

		void TextureOpenGL::SetColourPalette(int paletteIndex)
		{
#if defined ION_PLATFORM_DREAMCAST
			OpenGLContextStackLock lock;

			m_paletteIdx = paletteIndex;

			glBindTexture(GL_TEXTURE_2D, m_glTextureId);
			glColorBank(GL_TEXTURE_2D, GL_COLOR_TABLE + paletteIndex);
			glBindTexture(GL_TEXTURE_2D, 0);
#endif
		}

#if defined ION_GL_SUPPORT_FENCE
		GLsync TextureOpenGL::CreateFence()
		{
			OpenGLContextStackLock lock;

			return OpenGLExt::glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		}

		void TextureOpenGL::WaitFence(GLsync& sync)
		{
			OpenGLContextStackLock lock;

			bool waiting = true;
			while (waiting)
			{
				GLenum waitReturn = OpenGLExt::glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
				waiting = (waitReturn != GL_ALREADY_SIGNALED && waitReturn != GL_CONDITION_SATISFIED);
			}
		}

		void TextureOpenGL::DestroyFence(GLsync& fence)
		{
			OpenGLContextStackLock lock;

			OpenGLExt::glDeleteSync(fence);
		}
#endif

		void TextureOpenGL::ConvertTwiddled(const u8* src, u8*& dst, int imgWidth, int x1, int y1, int size)
		{
			if (size == 1)
			{
				*dst++ = src[y1*imgWidth + x1];
			}
			else
			{
				int ns = size / 2;
				ConvertTwiddled(src, dst, imgWidth, x1, y1, ns);
				ConvertTwiddled(src, dst, imgWidth, x1, y1 + ns, ns);
				ConvertTwiddled(src, dst, imgWidth, x1 + ns, y1, ns);
				ConvertTwiddled(src, dst, imgWidth, x1 + ns, y1 + ns, ns);
			}
		}

		void TextureOpenGL::GetOpenGLMode(Format format, BitsPerPixel bitsPerPixel, int& mode, int& byteFormat, int& colourFormat, int& pixelSize)
		{
			switch (bitsPerPixel)
			{
			case BitsPerPixel::BPP8:
			{
				switch (format)
				{
#if !defined ION_RENDERER_KGL
				case Format::R:
					mode = GL_RED;
					byteFormat = GL_UNSIGNED_BYTE;
					colourFormat = GL_RED;
					pixelSize = 1;
					break;
				case Format::RGB:
					mode = GL_RGB8;
					byteFormat = GL_UNSIGNED_BYTE;
					colourFormat = GL_RGB;
					pixelSize = 1;
					break;
				case Format::RGBA:
					mode = GL_RGBA8;
					byteFormat = GL_UNSIGNED_BYTE;
					colourFormat = GL_RGB;
					pixelSize = 1;
					break;
#endif
				case Format::RGBA_Indexed:
					mode = GL_COLOR_INDEX;
					byteFormat = GL_UNSIGNED_BYTE;
					colourFormat = GL_COLOR_INDEX;
					pixelSize = 1;
					break;
				default:
					ion::debug::Error("Unsupported texture data format");
					break;
				}
				break;
			}
			case BitsPerPixel::BPP16:
			{
				switch (format)
				{
				case Format::RGB:
					mode = GL_RGB;
					byteFormat = GL_UNSIGNED_SHORT_4_4_4_4;
					colourFormat = GL_RGB;
					pixelSize = 2;
					break;
				case Format::RGBA:
					mode = GL_RGBA;
					byteFormat = GL_UNSIGNED_SHORT_4_4_4_4;
					colourFormat = GL_RGBA;
					pixelSize = 2;
					break;
				default:
					ion::debug::Error("Unsupported texture data format");
					break;
				}
				break;
			}
			case BitsPerPixel::BPP24:
			{
				switch (format)
				{
				case Format::RGB:
					mode = GL_RGB;
					byteFormat = GL_UNSIGNED_BYTE;
					colourFormat = GL_RGB;
					pixelSize = 3;
					break;
				case Format::RGBA:
					mode = GL_RGBA;
					byteFormat = GL_UNSIGNED_BYTE;
					colourFormat = GL_RGBA;
					pixelSize = 4;
					break;
#if !defined ION_RENDERER_KGL
				case Format::BGRA:
					mode = GL_BGRA;
					byteFormat = GL_UNSIGNED_BYTE;
					colourFormat = GL_RGBA;
					pixelSize = 4;
					break;
				case Format::RGBA_DXT5:
					mode = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
					byteFormat = GL_UNSIGNED_BYTE;
					colourFormat = GL_RGBA;
					pixelSize = 4;
					break;
#endif
				default:
					ion::debug::Error("Unsupported texture data format");
					break;
				}
				break;
			}
			case BitsPerPixel::BPP32:
			{
				switch (format)
				{
#if !defined ION_RENDERER_KGL
				case Format::RGB:
					mode = GL_RGB;
					byteFormat = GL_UNSIGNED_INT_10_10_10_2;
					colourFormat = GL_RGB;
					pixelSize = 4;
					break;
				case Format::RGBA:
					mode = GL_RGBA;
					byteFormat = GL_UNSIGNED_INT_8_8_8_8;
					colourFormat = GL_RGBA;
					pixelSize = 4;
					break;
				case Format::BGRA:
					mode = GL_BGRA;
					byteFormat = GL_UNSIGNED_INT_8_8_8_8;
					colourFormat = GL_RGBA;
					pixelSize = 4;
					break;
#endif
				default:
					ion::debug::Error("Unsupported texture data format");
					break;
				}
				break;
			}
			default:
				ion::debug::Error("Unsupported texture BPP");
				break;
			}
		}
	}
}
