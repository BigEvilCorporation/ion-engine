#include "BuildTexture.h"

#include <png.h>

namespace ion
{
	namespace render
	{
		Texture* Texture::Create()
		{
			return new TextureResource();
		}

		void Texture::RegisterSerialiseType(io::Archive& archive)
		{
			archive.RegisterPointerTypeStrict<Texture, TextureResource>("ion::render::Texture");
		}
	}

	namespace build
	{
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

		void ReadPNG(ion::io::File& pngFile, std::vector<u8>& imageData, u32& width, u32& height, ion::render::Texture::Format& sourceFormat, ion::render::Texture::BitsPerPixel& bitsPerPixel)
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
					sourceFormat = ion::render::Texture::Format::RGB;
					png_uint_32 retval = png_get_IHDR(png_ptr, info_ptr, &width, &height, &bitsPerChannel, &colourType, NULL, NULL, NULL);

					switch (colourType)
					{
					case PNG_COLOR_TYPE_RGB:
						sourceFormat = ion::render::Texture::Format::RGB;
						bitsPerPixel = (ion::render::Texture::BitsPerPixel)(bitsPerChannel * 3);
						break;
					case PNG_COLOR_TYPE_RGB_ALPHA:
						sourceFormat = ion::render::Texture::Format::RGBA;
						bitsPerPixel = (ion::render::Texture::BitsPerPixel)(bitsPerChannel * 4);
						break;
					}

					//Alloc image data
					u32 bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);
					imageData.resize(bytesPerRow * height);

					//Copy data
					png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
					if (row_pointers)
					{
						for (int i = 0; i < height; i++)
						{
							ion::memory::MemCopy(imageData.data() + (bytesPerRow * i), row_pointers[i], bytesPerRow);
						}
					}
				}

				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			}

			delete[] pngData;
		}

		int BuildTexture(const std::string& outputFilename, const std::string& inputFilename, TextureFiletype filetype)
		{
			ion::io::File inputFile(inputFilename, ion::io::File::OpenMode::Read);
			if (!inputFile.IsOpen())
			{
				debug::error << "Could not open texture file " << inputFilename << debug::end;
				return -1;
			}

			ion::io::File outputFile(outputFilename, ion::io::File::OpenMode::Write);
			if (!outputFile.IsOpen())
			{
				debug::error << "Could not open output file for writing: " << outputFilename << debug::end;
				return -1;
			}

			std::vector<u8> imageData;
			u32 width;
			u32 height;
			ion::render::Texture::Format sourceFormat;
			ion::render::Texture::BitsPerPixel bitsPerPixel;

			switch (filetype)
			{
				case TextureFiletype::BMP:
					break;
				case TextureFiletype::PNG:
					ReadPNG(inputFile, imageData, width, height, sourceFormat, bitsPerPixel);
					break;
			}

			if (imageData.size() == 0)
			{
				debug::error << "Error converting " << inputFilename << debug::end;
				return -1;
			}

			ion::render::Texture* texture = ion::render::Texture::Create();
			texture->SetSerialiseData(inputFilename, imageData, width, height, sourceFormat, bitsPerPixel);
			ion::io::Archive archive(outputFile, ion::io::Archive::Direction::Out);
			ion::render::Texture::RegisterSerialiseType(archive);
			archive.Serialise(texture);

			debug::log << "Written: " << outputFilename << debug::end;

			return 0;
		}
	}
}