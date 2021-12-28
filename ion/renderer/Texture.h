///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Texture.h
// Date:		19th December 2011
// Authors:		Matt Phillips
// Description:	Texture map
///////////////////////////////////////////////////

#pragma once

#include "core/Types.h"
#include "maths/Vector.h"
#include "core/io/Archive.h"
#include "renderer/Colour.h"
#include <string>
#include <vector>

namespace ion
{
	namespace render
	{
		class Texture
		{
		public:
			enum class Format
			{
				R,
				RGB,
				BGR,
				RGBA,
				RGBA_Indexed,
				BGRA,
				RGBA_DXT5
			};

			enum class BitsPerPixel
			{
				BPP8 = 8,
				BPP16 = 16,
				BPP24 = 24,
				BPP32 = 32
			};

			enum class Filter
			{
				Nearest,
				Linear,
				MipMapLinear
			};

			enum class Wrapping
			{
				Clamp,
				Repeat,
				Mirror
			};

			static Texture* Create();
			static Texture* Create(u32 width, u32 height);
			static Texture* Create(u32 width, u32 height, Format sourceFormat, Format destFormat, BitsPerPixel bitsPerPixel, bool generateMipmaps, bool generatePixelBuffer, const u8* data);

			virtual ~Texture();

			u32 GetWidth() const;
			u32 GetHeight() const;
			BitsPerPixel GetBitsPerPixel() const;

			virtual bool Load(u32 width, u32 height, Format sourceFormat, Format destFormat, BitsPerPixel bitsPerPixel, bool generateMipmaps, bool generatePixelBuffer, const u8* data) { return false; }

			//Indexed textures
			virtual void SetColourPalette(int paletteIndex) = 0;

			virtual void SetMinifyFilter(Filter filter) = 0;
			virtual void SetMagnifyFilter(Filter filter) = 0;
			virtual void SetWrapping(Wrapping wrapping) = 0;

			virtual void SetPixel(const ion::Vector2i& Position, const Colour& colour) = 0;
			virtual void SetPixels(Format sourceFormat, bool synchronised, const u8* data) = 0;
			virtual void GetPixels(const ion::Vector2i& Position, const ion::Vector2i& size, Format format, BitsPerPixel bitsPerPixel, u8* data) const = 0;
			virtual u8* LockPixelBuffer() = 0;
			virtual void UnlockPixelBuffer() = 0;

			//Serialise
			void SetSerialiseData(const std::string& filename, std::vector<u8>& data, u32 width, u32 height, Format sourceFormat, BitsPerPixel bpp);
			static void RegisterSerialiseType(io::Archive& archive);
			void Serialise(io::Archive& archive);

			//Stats
			static u32 GetTextureMemoryUsed() { return s_textureMemoryUsed; }

		protected:
			Texture();
			Texture(u32 width, u32 height);
			Texture(u32 width, u32 height, Format sourceFormat, Format destFormat, BitsPerPixel bitsPerPixel, bool generateMipmaps, bool generatePixelBuffer, const u8* data);

			virtual bool Load() { return false; }
			virtual void Unload() { }

			u32 m_width;
			u32 m_height;
			u32 m_pixelSize;
			Format m_sourceFormat;
			Format m_destFormat;
			BitsPerPixel m_bitsPerPixel;

			std::string m_imageFilename;
			std::vector<u8> m_imageData;

			//Memory stats
			static u32 s_textureMemoryUsed;
		};
	}
}