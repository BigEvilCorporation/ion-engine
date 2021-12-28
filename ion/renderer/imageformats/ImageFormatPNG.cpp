#include "ImageFormatPNG.h"

#if ION_RENDER_SUPPORTS_PNG

#include "core/io/Stream.h"
#include "core/memory/Memory.h"
#include <pngstruct.h>
#include <pnginfo.h>

namespace ion
{
	ImageFormatPNG::ImageFormatPNG()
	{

	}

	ImageFormatPNG::~ImageFormatPNG()
	{

	}

	bool ImageFormatPNG::Read(const std::string& filename)
	{
		//Read file
		ion::io::File pngFile(filename, ion::io::File::OpenMode::Read);
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
				png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING, NULL);
				if (!readError)
				{
					//Extract image metadata
					int colourType = -1;
					int bitsPerChannel = 0;
					png_uint_32 retval = png_get_IHDR(png_ptr, info_ptr, &m_width, &m_height, &bitsPerChannel, &colourType, NULL, NULL, NULL);

					m_bitsPerPixel = info_ptr->pixel_depth;

					// Copy palette
					if (colourType == PNG_COLOR_TYPE_PALETTE && (info_ptr->valid & PNG_INFO_PLTE))
					{
						m_palette.resize(info_ptr->num_palette);

						for (int i = 0; i < info_ptr->num_palette; i++)
						{
							m_palette[i] = Colour(info_ptr->palette[i].red, info_ptr->palette[i].green, info_ptr->palette[i].blue);
						}
					}

					//Alloc image data
					u32 bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);
					m_data.resize(bytesPerRow * m_height);

					//Copy data
					png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);
					if (row_pointers)
					{
						for (int i = 0; i < m_height; i++)
						{
							ion::memory::MemCopy(m_data.data() + (bytesPerRow * i), row_pointers[i], bytesPerRow);
						}
					}
				}

				png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			}

			delete[] pngData;
			return true;
		}

		return false;
	}

	bool ImageFormatPNG::Write(const std::string& filename)
	{
		ion::debug::error << "PNG writing not supported yet" << ion::debug::end;
		return false;
	};

	int ImageFormatPNG::GetWidth() const
	{
		return m_width;
	}

	int ImageFormatPNG::GetHeight() const
	{
		return m_height;
	}

	void ImageFormatPNG::SetDimensions(int width, int height)
	{

	}

	int ImageFormatPNG::GetPaletteSize() const
	{
		return m_palette.size();
	}

	ImageFormat::Colour ImageFormatPNG::GetPaletteEntry(int index) const
	{
		return m_palette[index];
	}
	void ImageFormatPNG::SetPaletteEntry(int index, const Colour& colour)
	{

	}

	u8 ImageFormatPNG::GetColourIndex(int x, int y) const
	{
		return m_data[(m_width * y) + x];
	}

	void ImageFormatPNG::SetColourIndex(int x, int y, u8 index)
	{

	}

	ImageFormat::Colour ImageFormatPNG::GetPixel(int x, int y) const
	{
		return GetPaletteEntry(GetColourIndex(x, y));
	}
}

#endif