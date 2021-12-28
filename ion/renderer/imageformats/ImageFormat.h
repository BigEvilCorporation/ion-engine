#pragma once

#include <string>
#include <vector>

#include <core/Types.h>
#include <core/io/File.h>

namespace ion
{
	class ImageFormat
	{
	public:
		struct Colour
		{
			Colour() { r = 0; g = 0; b = 0; a = 255; }
			Colour(u8 R, u8 G, u8 B) { r = R; g = G; b = B; a = 255; }
			Colour(u8 R, u8 G, u8 B, u8 A) { r = R; g = G; b = B; a = A; }

			void SetRed(u8 R) { r = R; }
			void SetGreen(u8 G) { g = G; }
			void SetBlue(u8 B) { b = B; }

			u8 GetRed() const { return r; }
			u8 GetGreen() const { return g; }
			u8 GetBlue() const { return b; }

			u8 r;
			u8 g;
			u8 b;
			u8 a;
		};

		static ImageFormat* CreateReader(const std::string& fileExtension);

		virtual ~ImageFormat() {}

		virtual bool Read(const std::string& filename) = 0;
		virtual bool Write(const std::string& filename) = 0;

		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;

		virtual int GetPaletteSize() const = 0;
		virtual Colour GetPaletteEntry(int index) const = 0;
		virtual void SetPaletteEntry(int index, const Colour& colour) = 0;
		virtual u8 GetColourIndex(int x, int y) const = 0;
		virtual Colour GetPixel(int x, int y) const = 0;

		virtual void SetDimensions(int width, int height) = 0;
		virtual void SetColourIndex(int x, int y, u8 index) = 0;
	};
}