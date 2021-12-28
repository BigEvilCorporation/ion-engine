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

#include "ImageFormat.h"

namespace ion
{
	class ImageFormatBMP : public ImageFormat
	{
	public:
		enum BitFormat
		{
			eUnspecified = 0,
			eMonochrome1Bit = 1,
			eIndexed16Colour = 4,
			eIndexed256Colour = 8,
			eRGB16 = 16,
			eRGB24 = 24,
			eRGB32 = 32
		};

		enum FileType
		{
			eTypeBMP = 19778
		};

		enum Resolution
		{
			eResolutionDefault = 2835
		};

		ImageFormatBMP();
		virtual ~ImageFormatBMP();

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

#pragma pack(push, 2)
		struct FileHeader
		{
			u16 fileType;
			u32 fileSize;
			u16 unused1;
			u16 unused2;
			u32 dataOffset;
		};

		struct BMPHeader
		{
			u32 headerSize;
			u32 imageWidth;
			u32 imageHeight;
			u16 numPlanes;
			u16 bitFormat;
			u32 compressionFlags;
			u32 dataSizeBytes;
			u32 resolutionX;
			u32 resolutionY;
			u32 numUsedColours;
			u32 importantColourCount;
		};
#pragma pack(pop)

		struct RGBQuad
		{
			u8 b;
			u8 g;
			u8 r;
			u8 reserved;

			bool operator == (const RGBQuad& rhs) const
			{
				return b == rhs.b && g == rhs.g && r == rhs.r;
			}
		};

		int m_width;
		int m_height;
		u8 m_bitsPerPixel;

		//Palette
		std::vector<RGBQuad> m_palette;

		//Pixel data
		std::vector<u8> m_data;
	};
}