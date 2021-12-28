///////////////////////////////////////////////////
// File:		BuildTexture.h
// Date:		15th February 2014
// Authors:		Matt Phillips
// Description:	Resource build tool
///////////////////////////////////////////////////

#pragma once

#include "renderer/Texture.h"

#include <string>

namespace ion
{
	namespace build
	{
		enum class TextureFiletype
		{
			BMP,
			PNG
		};

		int BuildTexture(const std::string& outputFilename, const std::string& inputFilename, TextureFiletype filetype);
	}

	namespace render
	{
		class TextureResource : public Texture
		{
		public:
			virtual bool Load(const std::string& filename) { return false; }
			virtual void SetColourPalette(int paletteIndex) {}
			virtual void SetMinifyFilter(Filter filter) {}
			virtual void SetMagnifyFilter(Filter filter) {}
			virtual void SetWrapping(Wrapping wrapping) {}
			virtual void SetPixel(const ion::Vector2i& position, const Colour& colour) {}
			virtual void SetPixels(Format sourceFormat, bool synchronised, const u8* data) {}
			virtual void GetPixels(const ion::Vector2i& position, const ion::Vector2i& size, Format format, BitsPerPixel bitsPerPixel, u8* data) const {}
			virtual u8* LockPixelBuffer() { return nullptr; }
			virtual void UnlockPixelBuffer() {}
		};
	}
}