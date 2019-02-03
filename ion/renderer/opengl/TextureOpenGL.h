///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		TextureOpenGL.h
// Date:		13th December 2013
// Authors:		Matt Phillips
// Description:	OpenGL texture implementation
///////////////////////////////////////////////////

#pragma once

#include "core/Platform.h"
#include "renderer/Texture.h"
#include "OpenGLInclude.h"

#include <string>

#if ION_PLATFORM_WINDOWS
#define ION_GL_SUPPORT_FENCE
#endif

namespace ion
{
	namespace render
	{
		class TextureOpenGL : public Texture
		{
		public:
			TextureOpenGL();
			TextureOpenGL(u32 width, u32 height);
			TextureOpenGL(u32 width, u32 height, Format sourceFormat, Format destFormat, BitsPerPixel bitsPerPixel, bool generateMipmaps, bool generatePixelBuffer, const u8* data);
			virtual ~TextureOpenGL();

			virtual bool Load(const std::string& filename);
			virtual bool Load(u32 width, u32 height, Format sourceFormat, Format destFormat, BitsPerPixel bitsPerPixel, bool generateMipmaps, bool generatePixelBuffer, const u8* data);
			GLuint GetTextureId() const;

			//Indexed textures
			virtual void SetColourPalette(int paletteIndex);

			virtual void SetMinifyFilter(Filter filter);
			virtual void SetMagnifyFilter(Filter filter);
			virtual void SetWrapping(Wrapping wrapping);

			virtual void SetPixel(const ion::Vector2i& position, const Colour& colour);
			virtual void SetPixels(Format sourceFormat, bool synchronised, u8* data);
			virtual void GetPixels(const ion::Vector2i& position, const ion::Vector2i& size, Format format, BitsPerPixel bitsPerPixel, u8* data) const;
			virtual u8* LockPixelBuffer();
			virtual void UnlockPixelBuffer();

			static void GetOpenGLMode(Format format, BitsPerPixel bitsPerPixel, int& mode, int& byteFormat, int& colourFormat, int& pixelSize);

		protected:
			virtual bool Load();
			virtual void Unload();

#if defined ION_GL_SUPPORT_FENCE
			GLsync CreateFence();
			void DestroyFence(GLsync& fence);
			void WaitFence(GLsync& sync);
#endif

			void ConvertTwiddled(const u8* src, u8*& dst, int imgWidth, int x1, int y1, int size);

			GLuint m_glTextureId;
			GLuint m_glPixelBufferId;
			int m_glFormat;

			int m_paletteIdx;

#if defined ION_GL_SUPPORT_FENCE
			GLsync m_glSyncObject;
			u8* m_persistentBuffer;
#endif
		};
	}
}
