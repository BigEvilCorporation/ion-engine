///////////////////////////////////////////////////////
// Beehive: A complete SEGA Mega Drive content tool
//
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// Reader for indexed 16 colour, 4bpp BMP files
///////////////////////////////////////////////////////

#pragma once

#if ION_RENDER_SUPPORTS_PNG

#include "ImageFormat.h"
#include "core/debug/Debug.h"
#include <png.h>

namespace ion
{
	class ImageFormatPNG : public ImageFormat
	{
	public:
		ImageFormatPNG();
		virtual ~ImageFormatPNG();

		virtual bool Read(const std::string& filename);
		virtual bool Write(const std::string& filename);

		virtual int GetWidth() const;
		virtual int GetHeight() const;

		virtual void SetDimensions(int width, int height);

		virtual int GetPaletteSize() const;
		virtual Colour GetPaletteEntry(int index) const;
		virtual void SetPaletteEntry(int index, const Colour& colour);

		virtual u8 GetColourIndex(int x, int y) const;
		virtual void SetColourIndex(int x, int y, u8 index);
		virtual Colour GetPixel(int x, int y) const;

	private:
		static void MemoryReadPNG(png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
		{
			ion::io::MemoryStream* stream = (ion::io::MemoryStream*)png_get_io_ptr(png_ptr);
			stream->Read(outBytes, byteCountToRead);
		}

		static void ErrorHandlerPNG(png_structp png_ptr, png_const_charp message)
		{
			bool* readError = (bool*)png_get_error_ptr(png_ptr);
			*readError = true;
			ion::debug::error << "libpng error: " << message << ion::debug::end;
		}

		u32 m_width;
		u32 m_height;
		u8 m_bitsPerPixel;

		//Palette
		std::vector<Colour> m_palette;

		//Pixel data
		std::vector<u8> m_data;
	};
}

#endif