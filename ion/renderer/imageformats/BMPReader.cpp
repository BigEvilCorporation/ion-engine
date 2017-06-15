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

#include <ion/core/debug/Debug.h>

#include "BMPReader.h"

#include <algorithm>

BMPReader::BMPReader()
{
	m_width = 0;
	m_height = 0;
	m_bitsPerPixel = 0;
}

BMPReader::~BMPReader()
{

}

bool BMPReader::Read(const std::string& filename)
{
	ion::io::File file(filename);
	if(file.IsOpen())
	{
		//Read file header
		FileHeader fileHeader = {0};
		if(file.Read(&fileHeader, sizeof(FileHeader)))
		{
			//Read BMP header
			BMPHeader bmpHeader = {0};
			if(file.Read(&bmpHeader, sizeof(BMPHeader)))
			{
				//Sanity check size
				if(bmpHeader.imageWidth > 0 || bmpHeader.imageHeight > 0)
				{
					//Set width/height
					m_width = bmpHeader.imageWidth;
					m_height = bmpHeader.imageHeight;

					//Set bits per pixel
					m_bitsPerPixel = bmpHeader.bitFormat;

					//Seek to data pos
					if(file.Seek(fileHeader.dataOffset, ion::io::eSeekModeStart) == fileHeader.dataOffset)
					{
						//Alloc data buffer
						std::vector<u8> imageData;
						imageData.resize(bmpHeader.dataSizeBytes);

						//Read pixel data
						if(file.Read(imageData.data(), bmpHeader.dataSizeBytes) == bmpHeader.dataSizeBytes)
						{
							//Reserve indexed data buffer
							m_data.resize(m_width * m_height);

							//Determine palette type
							if(m_bitsPerPixel >= eIndexed16Colour && m_bitsPerPixel <= eIndexed256Colour)
							{
								//Indexed image, data already in correct format
								m_data = std::move(imageData);
								
								//Seek to palette pos
								u64 paletteDataPos = sizeof(FileHeader) + bmpHeader.headerSize;
								if(file.Seek(paletteDataPos, ion::io::eSeekModeStart) == paletteDataPos)
								{
									//Calc palette size and alloc
									m_palette.resize(bmpHeader.importantColourCount);

									//Read palette
									u64 paletteSizeBytes = bmpHeader.importantColourCount * sizeof(RGBQuad);
									if(file.Read(m_palette.data(), paletteSizeBytes) == paletteSizeBytes)
									{
										//Success
										file.Close();
										return true;
									}
								}
							}
							else if(m_bitsPerPixel >= eRGB16 && m_bitsPerPixel <= eRGB32)
							{
								//Seek to colour mask pos
								u64 paletteDataPos = sizeof(FileHeader) + bmpHeader.headerSize;
								u32 colourMaskR = 0;
								u32 colourMaskG = 0;
								u32 colourMaskB = 0;
								if(file.Seek(paletteDataPos, ion::io::eSeekModeStart) == paletteDataPos)
								{
									file.Read(&colourMaskG, sizeof(u32));
									file.Read(&colourMaskR, sizeof(u32));
									file.Read(&colourMaskB, sizeof(u32));
								}

								//RGB image, extract all pixels as RGB32
								const u8* dataPtr = imageData.data();

								for(int y = 0; y < m_height; y++)
								{
									for(int x = 0; x < m_width; x++)
									{
										//Extract pixel
										RGBQuad pixelColour;

										switch(m_bitsPerPixel)
										{
											case eRGB16:
											{
												if(colourMaskR)
												{
													const u16& data = *(const u16*)dataPtr++;
													pixelColour.r = (data >> 10) & colourMaskR;
													pixelColour.g = (data >> 5) & colourMaskG;
													pixelColour.b = (data) & colourMaskB;
												}
												else
												{
													//RGB16 = 0RRRRRGGGGGBBBBB
													const u16& data = *(const u16*)dataPtr++;
													pixelColour.r = (data >> 10) & 0x1F;
													pixelColour.g = (data >> 5) & 0x1F;
													pixelColour.b = (data)& 0x1F;
												}
												
												break;
											}
											case eRGB24:
											{
												//RGB24 = BBBBBBBBGGGGGGGGRRRRRRRR
												pixelColour.r = *dataPtr++;
												pixelColour.g = *dataPtr++;
												pixelColour.b = *dataPtr++;
												break;
											}
											case eRGB32:
											{
												//RGB32 = 00000000RRRRRRRRGGGGGGGGBBBBBBBB
												pixelColour.r = *dataPtr++;
												pixelColour.g = *dataPtr++;
												pixelColour.b = *dataPtr++;
												*dataPtr++;
												break;
											}
										};

										//Build unique colour palette
										int colourIndex = 0;
										std::vector<RGBQuad>::iterator it = std::find(m_palette.begin(), m_palette.end(), pixelColour);
										if(it == m_palette.end())
										{
											//New colour, add to palette
											m_palette.push_back(pixelColour);
											colourIndex = m_palette.size() - 1;
										}
										else
										{
											//Colour found
											colourIndex = std::distance(m_palette.begin(), it);
										}

										//Add to indexed data
										m_data[(y * m_width) + x] = colourIndex;
									}
								}

								//Success
								file.Close();
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

bool BMPReader::Write(const std::string& filename)
{
	ion::io::File file(filename, ion::io::File::eOpenWrite);
	if(file.IsOpen())
	{
		u64 paletteSizeBytes = m_palette.size() * sizeof(RGBQuad);

		//Write file header
		FileHeader fileHeader = { 0 };
		fileHeader.fileType = eTypeBMP;
		fileHeader.fileSize = sizeof(FileHeader) + sizeof(BMPHeader) + paletteSizeBytes + m_data.size();
		fileHeader.dataOffset = sizeof(FileHeader) + sizeof(BMPHeader) + paletteSizeBytes;
		file.Write(&fileHeader, sizeof(FileHeader));

		//Write BMP header
		BMPHeader bmpHeader = { 0 };
		bmpHeader.imageWidth = m_width;
		bmpHeader.imageHeight = m_height;
		bmpHeader.dataSizeBytes = m_data.size();
		bmpHeader.bitFormat = 4;
		bmpHeader.headerSize = sizeof(BMPHeader);
		bmpHeader.numPlanes = 1;
		bmpHeader.compressionFlags = 0;
		bmpHeader.resolutionX = eResolutionDefault;
		bmpHeader.resolutionY = eResolutionDefault;
		bmpHeader.importantColourCount = m_palette.size();
		bmpHeader.numUsedColours = m_palette.size();

		file.Write(&bmpHeader, sizeof(BMPHeader));

		//Write palette
		file.Write(m_palette.data(), paletteSizeBytes);

		//Write pixel data
		file.Write(m_data.data(), m_data.size());

		file.Close();

		//Success
		return true;
	}

	return false;
}

int BMPReader::GetWidth() const
{
	return m_width;
}

int BMPReader::GetHeight() const
{
	return m_height;
}

void BMPReader::SetDimensions(int width, int height)
{
	m_width = width;
	m_height = height;
	m_data.resize(width * height);
}

int BMPReader::GetPaletteSize() const
{
	return m_palette.size();
}

u8 BMPReader::GetColourIndex(int x, int y) const
{
	ion::debug::Assert(x < m_width && y < m_height, "Out of range");

	u32 pixelOffset = (((m_height - y - 1) * m_width) + x);
	u32 byteOffset = (pixelOffset & 0xFFFFFFFE) / 2;
	u8 colourIndex = m_data[byteOffset];

	if(pixelOffset & 1)
		colourIndex &= 0x0F;
	else
		colourIndex = colourIndex >> 4;

	ion::debug::Assert(colourIndex < m_palette.size(), "Out of range");

	return colourIndex;
}

void BMPReader::SetColourIndex(int x, int y, u8 index)
{
	ion::debug::Assert(x < m_width && y < m_height, "Out of range");
	ion::debug::Assert(index < m_palette.size(), "Out of range");

	u32 pixelOffset = (((m_height - y - 1) * m_width) + x);
	u32 byteOffset = (pixelOffset & 0xFFFFFFFE) / 2;
	u8& colourIndex = m_data[byteOffset];

	if((pixelOffset & 1) == 0)
		index = index << 4;

	colourIndex |= index;
}

BMPReader::Colour BMPReader::GetPaletteEntry(int index) const
{
	ion::debug::Assert(index < m_palette.size(), "Out of range");

	RGBQuad rgbQuad = m_palette[index];
	Colour colour;
	colour.SetRed(rgbQuad.r);
	colour.SetGreen(rgbQuad.g);
	colour.SetBlue(rgbQuad.b);
	return colour;
}

void BMPReader::SetPaletteEntry(int index, const Colour& colour)
{
	if(index >= m_palette.size())
	{
		m_palette.resize(index + 1);
	}

	m_palette[index].r = colour.GetRed();
	m_palette[index].g = colour.GetGreen();
	m_palette[index].b = colour.GetBlue();
}

BMPReader::Colour BMPReader::GetPixel(int x, int y) const
{
	ion::debug::Assert(x < m_width && y < m_height, "Out of range");

	u32 pixelOffset = (((m_height - y - 1) * m_width) + x);
	u32 byteOffset = (pixelOffset & 0xFFFFFFFE) / 2;
	u8 colourIndex = m_data[byteOffset];

	if (pixelOffset & 1)
		colourIndex &= 0x0F;
	else
		colourIndex = colourIndex >> 4;

	ion::debug::Assert(colourIndex < m_palette.size(), "Out of range");

	return GetPaletteEntry(colourIndex);
}
