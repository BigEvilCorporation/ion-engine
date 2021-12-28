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
#include "core/string/String.h"
#include "renderer/opengl/TextureOpenGL.h"
#include "renderer/opengl/RendererOpenGL.h"
#include "renderer/opengl/OpenGLExtensions.h"
#include "renderer/sdl/SDLInclude.h"

#if ION_RENDER_SUPPORTS_PNG
#include "core/io/Stream.h"
#include <png.h>
#endif

namespace ion
{
	namespace render
	{
#if ION_RENDER_SUPPORTS_PNG
		void MemoryReadPNG(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
		{
			ion::io::MemoryStream* stream = (ion::io::MemoryStream*)png_get_io_ptr(png_ptr);
			stream->Read(outBytes, byteCountToRead);
		}

		void ErrorHandlerPNG(png_structp png_ptr, png_const_charp message)
		{
			bool* readError = (bool*)png_get_error_ptr(png_ptr);
			*readError = true;
			ion::debug::error << "libpng error: " << message << ion::debug::end;
		}
#endif

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
			archive.RegisterPointerTypeStrict<Texture, TextureOpenGL>("ion::render::Texture");
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
#if defined ION_GL_SUPPORT_PIXEL_BUFFER_OBJECT
				//If supported
				const bool pboSupported =	(opengl::extensions->glMapBufferRange != nullptr)
										&& (opengl::extensions->glGenBuffers != nullptr)
										&& (opengl::extensions->glBufferStorage != nullptr)
										&& (opengl::extensions->glBindBuffer != nullptr);

				if (pboSupported)
				{
					//Generate pixel buffer
					debug::Assert(opengl::extensions->glGenBuffers != nullptr, "Cannot create pixel buffer without glGenBuffersARB extension");
					opengl::extensions->glGenBuffers(1, &m_glPixelBufferId);
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

			//TODO: Image loaders as plugins

#if ION_RENDER_SUPPORTS_PNG
			if (ion::string::EndsWith(ion::string::ToLower(m_imageFilename), ".png"))
			{
				//Read file
				ion::io::File pngFile(m_imageFilename, ion::io::File::OpenMode::Read);
				if (pngFile.IsOpen())
				{
					s64 pngSize = pngFile.GetSize();
					png_byte* pngData = new png_byte[pngSize];
					pngFile.Read(pngData, pngSize);

					//Create new PNG reader
					bool readError = false;
					png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, &readError, ErrorHandlerPNG, NULL);
					png_infop info_ptr = png_create_info_struct(png_ptr);
					if (png_ptr && info_ptr)
					{
						//Setup stream reader
						ion::io::MemoryStream stream(pngData, pngSize);
						png_set_read_fn(png_ptr, &stream, MemoryReadPNG);

						//Read PNG data
						png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);
						if (!readError)
						{
							//Extract image metadata
							int colourType = -1;
							int bitsPerChannel = 0;
							Format sourceFormat = Format::RGB;
							png_uint_32 retval = png_get_IHDR(png_ptr, info_ptr, &m_width, &m_height, &bitsPerChannel, &colourType, NULL, NULL, NULL);

							switch (colourType)
							{
							case PNG_COLOR_TYPE_RGB:
								sourceFormat = Format::RGB;
								m_bitsPerPixel = (BitsPerPixel)(bitsPerChannel * 3);
								break;
							case PNG_COLOR_TYPE_RGB_ALPHA:
								sourceFormat = Format::RGBA;
								m_bitsPerPixel = (BitsPerPixel)(bitsPerChannel * 4);
								break;
							}

							//Alloc image data
							u32 bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);
							u8* imageData = new u8[bytesPerRow * m_height];

							//Copy data
							png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
							if (row_pointers)
							{
								for (int i = 0; i < m_height; i++)
								{
									ion::memory::MemCopy(imageData + (bytesPerRow * i), row_pointers[i], bytesPerRow);
								}

								//Create OpenGL texture
								Load(m_width, m_height, sourceFormat, Format::RGBA, m_bitsPerPixel, true, false, imageData);
							}

							delete[] imageData;
						}

						png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
					}

					delete[] pngData;

					if (m_glTextureId != 0)
					{
						return true;
					}
				}
			}
#endif

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
			GetOpenGLMode(sourceFormat, bitsPerPixel, m_glFormatSrc, glByteFormat, glColourFormat, pixelSize);
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
				converted = true;
			}
#endif

			//Set default filters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

#if !defined ION_RENDERER_KGL
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#endif

			//Generate mipmaps
#if defined ION_RENDERER_OPENGL_ES
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
#elif defined ION_RENDERER_KGL
			//TODO
#elif defined ION_RENDERER_FIXED
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, generateMipmaps ? GL_TRUE : GL_FALSE);
#elif defined ION_RENDERER_SHADER
			if (generateMipmaps && opengl::extensions->glGenerateMipmap != nullptr)
			{
				opengl::extensions->glGenerateMipmap(GL_TEXTURE_2D);
			}
#endif

			//Set size
			m_width = width;
			m_height = height;

			//Copy all pixels in surface to GL texture
			glTexImage2D(GL_TEXTURE_2D, 0, glColourFormat, width, height, 0, m_glFormatSrc, glByteFormat, data);
			RendererOpenGL::CheckGLError("TextureOpenGL::Load");

			if (m_glPixelBufferId)
			{
#if defined ION_GL_SUPPORT_PIXEL_BUFFER_OBJECT
				//Bind the pixel buffer
				opengl::extensions->glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, m_glPixelBufferId);

				//Create data container
				opengl::extensions->glBufferStorage(GL_PIXEL_UNPACK_BUFFER_EXT, width * height * pixelSize, 0, GL_DYNAMIC_STORAGE_BIT | GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

				//Map buffer persistently
				m_persistentBuffer = (u8*)opengl::extensions->glMapBufferRange(GL_PIXEL_UNPACK_BUFFER_EXT, 0, width * height * pixelSize, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

				//Copy pixels
				if (data)
				{
					ion::memory::MemCopy(m_persistentBuffer, data, width * height * pixelSize);
				}

				//Unbind pixel buffer
				opengl::extensions->glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, 0);

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
				RendererOpenGL::CheckGLError("TextureOpenGL::Unload");
				m_glTextureId = 0;

				//Update stats
				s_textureMemoryUsed -= (m_width * m_height * m_pixelSize);
			}

			if (m_glPixelBufferId)
			{
#if defined ION_GL_SUPPORT_PIXEL_BUFFER_OBJECT
				//Destroy fence
				DestroyFence(m_glSyncObject);

				//Unmap
#if 0
				if (!opengl::extensions->glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_EXT))
				{
					debug::error << "TextureOpenGL::Unload() - Could not unmap PBO" << debug::end;
				}
#endif

				opengl::extensions->glDeleteBuffers(1, &m_glPixelBufferId);
				RendererOpenGL::CheckGLError("TextureOpenGL::Unload");
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

		void TextureOpenGL::SetPixels(Format sourceFormat, bool synchronised, const u8* data)
		{
			OpenGLContextStackLock lock;

#if !defined ION_RENDERER_KGL
			GLint glMode;
			GLint glByteFormat;
			GLint glColourFormat;
			int pixelSize = 0;
			GetOpenGLMode(sourceFormat, m_bitsPerPixel, glMode, glByteFormat, glColourFormat, pixelSize);

			if (m_glPixelBufferId)
			{
#if defined ION_GL_SUPPORT_PIXEL_BUFFER_OBJECT
				//Bind pixel buffer
				opengl::extensions->glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, m_glPixelBufferId);

				//Bind texture
				glBindTexture(GL_TEXTURE_2D, m_glTextureId);

				//Get buffer data ptr
				u8* pixelBufferData = nullptr;
				if (synchronised)
				{
					pixelBufferData = (unsigned char*)opengl::extensions->glMapBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, GL_WRITE_ONLY);
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
				opengl::extensions->glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, 0);
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

#if !defined ION_RENDERER_KGL && !defined ION_RENDERER_OPENGL_ES
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

#if defined ION_GL_SUPPORT_PIXEL_BUFFER_OBJECT
			if (m_persistentBuffer)
			{
				GLint glMode;
				GLint glByteFormat;
				GLint glColourFormat;
				int pixelSize = 0;
				GetOpenGLMode(Format::RGB, m_bitsPerPixel, glMode, glByteFormat, glColourFormat, pixelSize);

				//Bind pixel buffer
				opengl::extensions->glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, m_glPixelBufferId);

				//Bind texture
				glBindTexture(GL_TEXTURE_2D, m_glTextureId);

				//Copy texture to buffer
				glGetTexImage(GL_TEXTURE_2D, 0, glMode, glByteFormat, m_persistentBuffer);

				//Unbind texture
				glBindTexture(GL_TEXTURE_2D, 0);

				//Unbind buffer
				opengl::extensions->glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, 0);

				return m_persistentBuffer;
			}
			else
			{
				//Get buffer data ptr
				return (u8*)opengl::extensions->glMapBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, GL_READ_WRITE);
			}
#else
			return nullptr;
#endif
		}

		void TextureOpenGL::UnlockPixelBuffer()
		{
			OpenGLContextStackLock lock;

			ion::debug::Assert(m_glPixelBufferId, "TextureOpenGL::UnlockPixelBuffer() - Not a PBO");

#if defined ION_GL_SUPPORT_PIXEL_BUFFER_OBJECT
			GLint glMode;
			GLint glByteFormat;
			GLint glColourFormat;
			int pixelSize = 0;
			GetOpenGLMode(Format::RGB, m_bitsPerPixel, glMode, glByteFormat, glColourFormat, pixelSize);

			//Bind pixel buffer
			opengl::extensions->glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, m_glPixelBufferId);

			//Bind texture
			glBindTexture(GL_TEXTURE_2D, m_glTextureId);

			//Copy buffer to texture
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_width, m_height, glMode, glByteFormat, 0);

			//Unbind texture
			glBindTexture(GL_TEXTURE_2D, 0);

			//Unbind buffer
			opengl::extensions->glBindBuffer(GL_PIXEL_UNPACK_BUFFER_EXT, 0);
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
#if OPENGL_MAJOR_VERSION >= 4
				wrapMode = GL_CLAMP_TO_EDGE;
#else
				wrapMode = GL_CLAMP;
#endif
				break;
			case Wrapping::Repeat:
#if defined ION_PLATFORM_RASPBERRYPI
				wrapMode = GL_MIRRORED_REPEAT;
#else
				wrapMode = GL_REPEAT;
#endif
				break;
			case Wrapping::Mirror:
#if !defined ION_RENDERER_KGL && !defined ION_RENDERER_OPENGL_ES
				wrapMode = GL_MIRRORED_REPEAT;
#endif
				break;
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
			//glColorBankEXT(GL_TEXTURE_2D, GL_COLOR_TABLE + paletteIndex);
			glBindTexture(GL_TEXTURE_2D, 0);
#endif
		}

#if defined ION_GL_SUPPORT_FENCE
		GLsync TextureOpenGL::CreateFence()
		{
			OpenGLContextStackLock lock;

			return opengl::extensions->glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		}

		void TextureOpenGL::WaitFence(GLsync& sync)
		{
			OpenGLContextStackLock lock;

			bool waiting = true;
			while (waiting)
			{
				GLenum waitReturn = opengl::extensions->glClientWaitSync(sync, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
				waiting = (waitReturn != GL_ALREADY_SIGNALED && waitReturn != GL_CONDITION_SATISFIED);
			}
		}

		void TextureOpenGL::DestroyFence(GLsync& fence)
		{
			OpenGLContextStackLock lock;

			opengl::extensions->glDeleteSync(fence);
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
#if !defined ION_RENDERER_KGL && !defined ION_RENDERER_OPENGL_ES
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
#if defined ION_RENDERER_KGL
				case Format::RGBA_Indexed:
					mode = GL_COLOR_INDEX;
					byteFormat = GL_UNSIGNED_BYTE_TWID_KOS;
					colourFormat = GL_COLOR_INDEX8_EXT;
					pixelSize = 1;
					break;
#endif
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
#if !defined ION_RENDERER_OPENGL_ES
				case Format::RGBA_DXT5:
					mode = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
					byteFormat = GL_UNSIGNED_BYTE;
					colourFormat = GL_RGBA;
					pixelSize = 4;
					break;
#endif
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
#if !defined ION_RENDERER_KGL && !defined ION_RENDERER_OPENGL_ES
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
