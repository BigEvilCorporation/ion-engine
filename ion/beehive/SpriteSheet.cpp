///////////////////////////////////////////////////////
// Beehive: A complete SEGA Mega Drive content tool
//
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
///////////////////////////////////////////////////////

#include <ion/core/string/String.h>
#include <ion/core/debug/Debug.h>

#include "SpriteSheet.h"
#include "Stamp.h"
#include "renderer/imageformats/ImageFormatBMP.h"

#define HEX1(val) std::hex << std::setfill('0') << std::setw(1) << std::uppercase << (u16)val
#define HEX2(val) std::hex << std::setfill('0') << std::setw(2) << std::uppercase << (u16)val
#define HEX4(val) std::hex << std::setfill('0') << std::setw(4) << std::uppercase << (u16)val

const int SpriteSheet::s_subSpriteWidthTiles = 4;
const int SpriteSheet::s_subSpriteHeightTiles = 4;

SpriteSheet::SpriteSheet()
{
	m_widthTiles = 0;
	m_heightTiles = 0;
}

bool SpriteSheet::ImportBitmap(const std::string& filename, const std::string& name, int tileWidth, int tileHeight, int widthFrames, int heightFrames, int startFrame, int endFrame)
{
	//Clear frames
	m_frames.clear();

	//Read BMP
	ion::ImageFormat* reader = ion::ImageFormat::CreateReader(ion::string::GetFileExtension(filename));
	if(reader && reader->Read(filename))
	{
		if(reader->GetWidth() % tileWidth != 0 || reader->GetHeight() % tileHeight != 0)
		{
			//TODO: wx message handler in ion::debug
			//if(wxMessageBox("Bitmap width/height is not multiple of target platform tile width/height", "Warning", wxOK | wxCANCEL | wxICON_WARNING) == wxCANCEL)
			{
				return false;
			}
		}

		//Clear palette
		m_palette = Palette();

		//Import palette
		for(int i = 0; i < reader->GetPaletteSize(); i++)
		{
			ion::ImageFormat::Colour bmpColour = reader->GetPaletteEntry(i);
			m_palette.AddColour(Colour(bmpColour.r, bmpColour.g, bmpColour.b));
		}
	
		//Get total spriteSheet sheet width/height in tiles
		int spriteSheetWidthTiles = reader->GetWidth() / tileWidth;
		int spriteSheetHeightTiles = reader->GetHeight() / tileHeight;
	
		//Get frame width/height in tiles
		m_widthTiles = spriteSheetWidthTiles / widthFrames;
		m_heightTiles = spriteSheetHeightTiles / heightFrames;
	
		//For each frame
		u32 frameCount = startFrame;
		int startFrameX = startFrame % widthFrames;
		int startFrameY = startFrame / widthFrames;

		for(int frameY = startFrameY; frameY < heightFrames && frameCount <= endFrame; frameY++)
		{
			for(int frameX = startFrameX; frameX < widthFrames && frameCount <= endFrame; frameX++)
			{
				//Create new frame
				SpriteSheetFrame frame;
	
				//For each tile in frame (Mega Drive order = column major)
				for(int tileX = 0; tileX < m_widthTiles; tileX++)
				{
					for(int tileY = 0; tileY < m_heightTiles; tileY++)
					{
						//Read tile
						Tile tile(tileWidth, tileHeight);

						//Read pixel colours from bitmap
						for(int pixelX = 0; pixelX < tileWidth; pixelX++)
						{
							for(int pixelY = 0; pixelY < tileHeight; pixelY++)
							{
								//Read pixel
								int sourcePixelX = (frameX * m_widthTiles * tileWidth) + (tileX * tileWidth) + pixelX;
								int sourcePixelY = (frameY * m_heightTiles * tileHeight) + (tileY * tileHeight) + pixelY;
								u8 colourIndex = reader->GetColourIndex(sourcePixelX, sourcePixelY);
								tile.SetPixelColour(pixelX, pixelY, colourIndex);
							}
						}

						//Add tile
						frame.push_back(tile);
					}
				}

				//Add frame
				m_frames.push_back(frame);

				frameCount++;
			}
		}

		//Set name
		m_name = name;

		delete reader;
	}

	CropAllFrames(tileWidth, tileHeight);

	return true;
}

const std::string& SpriteSheet::GetName() const
{
	return m_name;
}

void SpriteSheet::SetName(const std::string& name)
{
	m_name = name;
}

const SpriteSheetFrame& SpriteSheet::GetFrame(int index) const
{
	ion::debug::Assert(index >= 0 && index < GetNumFrames(), "SpriteSheet::GetFrame() - eOut of range");
	return m_frames[index];
}

int SpriteSheet::GetNumFrames() const
{
	return m_frames.size();
}

int SpriteSheet::GetNumCroppedTiles(int tileWidth, int tileHeight) const
{
	int numTiles = 0;

	for(int i = 0; i < GetNumFrames(); i++)
	{
		numTiles += GetWidthTiles(i, tileWidth) * GetHeightTiles(i, tileHeight);
	}

	return numTiles;
}

SpriteAnimId SpriteSheet::AddAnimation()
{
	SpriteAnimId animId = ion::GenerateUUID64();
	m_animations.insert(std::make_pair(animId, SpriteAnimation()));
	return animId;
}

void SpriteSheet::DeleteAnimation(SpriteAnimId animId)
{
	m_animations.erase(animId);
}

SpriteAnimation* SpriteSheet::GetAnimation(SpriteAnimId animId)
{
	SpriteAnimation* anim = NULL;

	TSpriteAnimMap::iterator it = m_animations.find(animId);
	if(it != m_animations.end())
	{
		anim = &it->second;
	}

	return anim;
}

const SpriteAnimation* SpriteSheet::GetAnimation(SpriteAnimId animId) const
{
	const SpriteAnimation* anim = NULL;

	TSpriteAnimMap::const_iterator it = m_animations.find(animId);
	if(it != m_animations.end())
	{
		anim = &it->second;
	}

	return anim;
}

SpriteAnimation* SpriteSheet::FindAnimation(const std::string name)
{
	for(TSpriteAnimMap::iterator it = m_animations.begin(), end = m_animations.end(); it != end; ++it)
	{
		if(ion::string::CompareNoCase(it->second.GetName(), name))
		{
			return &it->second;
		}
	}

	return NULL;
}

const SpriteAnimation* SpriteSheet::FindAnimation(const std::string name) const
{
	for (TSpriteAnimMap::const_iterator it = m_animations.begin(), end = m_animations.end(); it != end; ++it)
	{
		if (ion::string::CompareNoCase(it->second.GetName(), name))
		{
			return &it->second;
		}
	}

	return NULL;
}

AnimationId SpriteSheet::FindAnimationId(const std::string name) const
{
	for (TSpriteAnimMap::const_iterator it = m_animations.begin(), end = m_animations.end(); it != end; ++it)
	{
		if (ion::string::CompareNoCase(it->second.GetName(), name))
		{
			return it->first;
		}
	}

	return InvalidAnimationId;
}

int SpriteSheet::GetNumAnimations() const
{
	return m_animations.size();
}

TSpriteAnimMap::const_iterator SpriteSheet::AnimationsBegin() const
{
	return m_animations.begin();
}

TSpriteAnimMap::const_iterator SpriteSheet::AnimationsEnd() const
{
	return m_animations.end();
}

TSpriteAnimMap::iterator SpriteSheet::AnimationsBegin()
{
	return m_animations.begin();
}

TSpriteAnimMap::iterator SpriteSheet::AnimationsEnd()
{
	return m_animations.end();
}

u8 SpriteSheet::GetWidthTiles() const
{
	return m_widthTiles;
}

u8 SpriteSheet::GetHeightTiles() const
{
	return m_heightTiles;
}

const SpriteSheet::SpriteFrameDimensions& SpriteSheet::GetDimensionData(int frameIdx) const
{
	return m_croppedDimensionData[frameIdx];
}

u8 SpriteSheet::GetWidthTiles(int frameIdx, int tileWidth) const
{
	return (m_croppedDimensionData[frameIdx].bottomRight.x - m_croppedDimensionData[frameIdx].topLeft.x) / tileWidth;
}

u8 SpriteSheet::GetHeightTiles(int frameIdx, int tileHeight) const
{
	return (m_croppedDimensionData[frameIdx].bottomRight.y - m_croppedDimensionData[frameIdx].topLeft.y) / tileHeight;
}

u8 SpriteSheet::GetMaxWidthTiles(int tileWidth) const
{
	int widthTiles = 0;

	for(int i = 0; i < m_croppedDimensionData.size(); i++)
	{
		int currFrameWidth = GetWidthTiles(i, tileWidth);
		if(currFrameWidth > widthTiles)
			widthTiles = currFrameWidth;
	}

	return widthTiles;
}

u8 SpriteSheet::GetMaxHeightTiles(int tileHeight) const
{
	int heightTiles = 0;

	for(int i = 0; i < m_croppedDimensionData.size(); i++)
	{
		int currFrameHeight = GetHeightTiles(i, tileHeight);
		if(currFrameHeight > heightTiles)
			heightTiles = currFrameHeight;
	}

	return heightTiles;
}

void SpriteSheet::GetWidthSubsprites(int frameIdx, int tileWidth, u8& total, u8& whole, u8& remainder) const
{
	int widthTiles = GetWidthTiles(frameIdx, tileWidth);
	total = ion::maths::Ceil((float)widthTiles / (float)s_subSpriteWidthTiles);
	whole = widthTiles / s_subSpriteWidthTiles;
	remainder = widthTiles % s_subSpriteWidthTiles;
}

void SpriteSheet::GetHeightSubsprites(int frameIdx, int tileHeight, u8& total, u8& whole, u8& remainder) const
{
	int heightTiles = GetHeightTiles(frameIdx, tileHeight);
	total = ion::maths::Ceil((float)heightTiles / (float)s_subSpriteHeightTiles);
	whole = heightTiles / s_subSpriteHeightTiles;
	remainder = heightTiles % s_subSpriteHeightTiles;
}

void SpriteSheet::GetSubspriteDimensions(u32 frameIdx, std::vector<ion::Vector2i>& dimensions, int tileWidth, int tileHeight) const
{
	int widthTiles = GetWidthTiles(frameIdx, tileWidth);
	int heightTiles = GetHeightTiles(frameIdx, tileHeight);
	int width = ion::maths::Ceil((float)widthTiles / (float)s_subSpriteWidthTiles);
	int height = ion::maths::Ceil((float)heightTiles / (float)s_subSpriteHeightTiles);

	for(int x = 0; x < width; x++)
	{
		for(int y = 0; y < height; y++)
		{
			int subSprWidth = ion::maths::Min(s_subSpriteWidthTiles, widthTiles - (s_subSpriteWidthTiles * x));
			int subSprHeight = ion::maths::Min(s_subSpriteHeightTiles, heightTiles - (s_subSpriteHeightTiles * y));
			dimensions.push_back(ion::Vector2i(subSprWidth, subSprHeight));
		}
	}
}

void SpriteSheet::GetSubspritePosOffsets(u32 frameIdx, std::vector<ion::Vector2i>& offsets, int tileWidth, int tileHeight) const
{

#if defined BEEHIVE_PLUGIN_LUMINARY
	const SpriteFrameDimensions& dimensionsData = GetDimensionData(frameIdx);
	int croppedWidthTiles = GetWidthTiles(frameIdx, tileWidth);
	int croppedHeightTiles = GetHeightTiles(frameIdx, tileHeight);
	int croppedWidth = ion::maths::Ceil((float)croppedWidthTiles / (float)s_subSpriteWidthTiles);
	int croppedHeight = ion::maths::Ceil((float)croppedHeightTiles / (float)s_subSpriteHeightTiles);

	int extentsX = (m_widthTiles * tileWidth) / 2;
	int extentsY = (m_heightTiles * tileHeight) / 2;

	int curSubSprX = 0;
	int curSubSprY = 0;

	for(int x = 0; x < croppedWidth; x++)
	{
		for(int y = 0; y < croppedHeight; y++)
		{
			int subSprX = ((x * s_subSpriteWidthTiles * tileWidth) + dimensionsData.topLeft.x - extentsX) - curSubSprX;
			int subSprY = ((y * s_subSpriteHeightTiles * tileHeight) + dimensionsData.topLeft.y - extentsY) - curSubSprY;
			curSubSprX += subSprX;
			curSubSprY += subSprY;
			offsets.push_back(ion::Vector2i(subSprX, subSprY));
		}
	}

#else

	const SpriteFrameDimensions& dimensionsData = GetDimensionData(frameIdx);
	int widthTiles = GetWidthTiles(frameIdx, tileWidth);
	int heightTiles = GetHeightTiles(frameIdx, tileHeight);
	int width = ion::maths::Ceil((float)widthTiles / (float)s_subSpriteWidthTiles);
	int height = ion::maths::Ceil((float)heightTiles / (float)s_subSpriteHeightTiles);

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			int subSprX = (x * s_subSpriteWidthTiles * tileWidth) + dimensionsData.topLeft.x;
			int subSprY = (y * s_subSpriteHeightTiles * tileHeight) + dimensionsData.topLeft.y;
			offsets.push_back(ion::Vector2i(subSprX, subSprY));
		}
	}

#endif // BEEHIVE_PLUGIN_LUMINARY
}

void SpriteSheet::GetSubspritePosOffsetsFlippedX(u32 frameIdx, std::vector<ion::Vector2i>& offsets, int tileWidth, int tileHeight) const
{
#if defined BEEHIVE_PLUGIN_LUMINARY

	const SpriteFrameDimensions& dimensionsData = GetDimensionData(frameIdx);
	int croppedWidthTiles = GetWidthTiles(frameIdx, tileWidth);
	int croppedHeightTiles = GetHeightTiles(frameIdx, tileHeight);
	int croppedWidth = ion::maths::Ceil((float)croppedWidthTiles / (float)s_subSpriteWidthTiles);
	int croppedHeight = ion::maths::Ceil((float)croppedHeightTiles / (float)s_subSpriteHeightTiles);

	int extentsX = (m_widthTiles * tileWidth) / 2;
	int extentsY = (m_heightTiles * tileHeight) / 2;

	int totalWidthPixels = m_widthTiles * tileWidth;
	int croppedWidthPixels = croppedWidthTiles * tileWidth;

	int curSubSprX = 0;
	int curSubSprY = 0;

	for(int x = 0; x < croppedWidth; x++)
	{
		for(int y = 0; y < croppedHeight; y++)
		{
			int subSprWidth = ion::maths::Min(s_subSpriteWidthTiles, croppedWidthTiles - (s_subSpriteWidthTiles * x)) * tileWidth;
			int subSprXOffset = x * s_subSpriteWidthTiles * tileWidth;
			int subSprX = (croppedWidthPixels - (subSprWidth + subSprXOffset) + (totalWidthPixels - croppedWidthPixels - dimensionsData.topLeft.x - extentsX)) - curSubSprX;
			int subSprY = ((y * s_subSpriteHeightTiles * tileHeight) + dimensionsData.topLeft.y - extentsY) - curSubSprY;
			curSubSprX += subSprX;
			curSubSprY += subSprY;
			offsets.push_back(ion::Vector2i(subSprX, subSprY));
		}
	}

#else

	const SpriteFrameDimensions& dimensionsData = GetDimensionData(frameIdx);
	int widthTiles = GetWidthTiles(frameIdx, tileWidth);
	int heightTiles = GetHeightTiles(frameIdx, tileHeight);
	int widthPixels = widthTiles * tileWidth;
	int width = ion::maths::Ceil((float)widthTiles / (float)s_subSpriteWidthTiles);
	int height = ion::maths::Ceil((float)heightTiles / (float)s_subSpriteHeightTiles);

	int totalWidthPixels = m_widthTiles * tileWidth;
	int croppedWidthPixels = widthTiles * tileWidth;

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			int subSprWidth = ion::maths::Min(s_subSpriteWidthTiles, widthTiles - (s_subSpriteWidthTiles * x)) * tileWidth;
			int subSprXOffset = x * s_subSpriteWidthTiles * tileWidth;
			int subSprX = widthPixels - (subSprWidth + subSprXOffset) + (totalWidthPixels - croppedWidthPixels - dimensionsData.topLeft.x);
			int subSprY = (y * s_subSpriteHeightTiles * tileHeight) + dimensionsData.topLeft.y;
			offsets.push_back(ion::Vector2i(subSprX, subSprY));
		}
	}

#endif // BEEHIVE_PLUGIN_LUMINARY
}

void SpriteSheet::GetSubspritePosOffsetsFlippedY(u32 frameIdx, std::vector<ion::Vector2i>& offsets, int tileWidth, int tileHeight) const
{
#if defined BEEHIVE_PLUGIN_LUMINARY
	const SpriteFrameDimensions& dimensionsData = GetDimensionData(frameIdx);
	int croppedWidthTiles = GetWidthTiles(frameIdx, tileWidth);
	int croppedHeightTiles = GetHeightTiles(frameIdx, tileHeight);
	int croppedWidth = ion::maths::Ceil((float)croppedWidthTiles / (float)s_subSpriteWidthTiles);
	int croppedHeight = ion::maths::Ceil((float)croppedHeightTiles / (float)s_subSpriteHeightTiles);

	int extentsX = (m_widthTiles * tileWidth) / 2;
	int extentsY = (m_heightTiles * tileHeight) / 2;

	int totalHeightPixels = m_heightTiles * tileHeight;
	int croppedHeightPixels = croppedHeightTiles * tileHeight;

	int curSubSprX = 0;
	int curSubSprY = 0;

	for (int x = 0; x < croppedWidth; x++)
	{
		int remainingHeight = croppedHeightTiles;

		for (int y = 0; y < croppedHeight; y++)
		{
			int subSprHeight = ion::maths::Min(s_subSpriteHeightTiles, remainingHeight) * tileHeight;
			int subSprYOffset = y * s_subSpriteHeightTiles * tileHeight;
			int subSprX = ((x * s_subSpriteWidthTiles * tileWidth) + dimensionsData.topLeft.x - extentsX) - curSubSprX;
			int subSprY = (croppedHeightPixels - (subSprHeight + subSprYOffset) + (totalHeightPixels - croppedHeightPixels - dimensionsData.topLeft.y - extentsY)) - curSubSprY;
			curSubSprX += subSprX;
			curSubSprY += subSprY;
			remainingHeight -= s_subSpriteHeightTiles;
			offsets.push_back(ion::Vector2i(subSprX, subSprY));
		}
	}
#endif
}

void SpriteSheet::GetSubspritePosOffsetsFlippedXY(u32 frameIdx, std::vector<ion::Vector2i>& offsets, int tileWidth, int tileHeight) const
{
	#if defined BEEHIVE_PLUGIN_LUMINARY
	const SpriteFrameDimensions& dimensionsData = GetDimensionData(frameIdx);
	int croppedWidthTiles = GetWidthTiles(frameIdx, tileWidth);
	int croppedHeightTiles = GetHeightTiles(frameIdx, tileHeight);
	int croppedWidth = ion::maths::Ceil((float)croppedWidthTiles / (float)s_subSpriteWidthTiles);
	int croppedHeight = ion::maths::Ceil((float)croppedHeightTiles / (float)s_subSpriteHeightTiles);

	int extentsX = (m_widthTiles * tileWidth) / 2;
	int extentsY = (m_heightTiles * tileHeight) / 2;

	int totalWidthPixels = m_widthTiles * tileWidth;
	int totalHeightPixels = m_heightTiles * tileHeight;
	int croppedWidthPixels = croppedWidthTiles * tileWidth;
	int croppedHeightPixels = croppedHeightTiles * tileHeight;

	int curSubSprX = 0;
	int curSubSprY = 0;

	for (int x = 0; x < croppedWidth; x++)
	{
		int remainingHeight = croppedHeightTiles;

		for (int y = 0; y < croppedHeight; y++)
		{
			int subSprWidth = ion::maths::Min(s_subSpriteWidthTiles, croppedWidthTiles - (s_subSpriteWidthTiles * x)) * tileWidth;
			int subSprHeight = ion::maths::Min(s_subSpriteHeightTiles, remainingHeight) * tileHeight;
			int subSprXOffset = x * s_subSpriteWidthTiles * tileWidth;
			int subSprYOffset = y * s_subSpriteHeightTiles * tileHeight;

			int subSprX = (croppedWidthPixels - (subSprWidth + subSprXOffset) + (totalWidthPixels - croppedWidthPixels - dimensionsData.topLeft.x - extentsX)) - curSubSprX;
			int subSprY = (croppedHeightPixels - (subSprHeight + subSprYOffset) + (totalHeightPixels - croppedHeightPixels - dimensionsData.topLeft.y - extentsY)) - curSubSprY;
			curSubSprX += subSprX;
			curSubSprY += subSprY;
			remainingHeight -= s_subSpriteHeightTiles;
			offsets.push_back(ion::Vector2i(subSprX, subSprY));
		}
	}
#endif
}

const Palette& SpriteSheet::GetPalette() const
{
	return m_palette;
}

void SpriteSheet::CropAllFrames(int tileWidth, int tileHeight)
{
	//Clear dimensions
	m_croppedDimensionData.clear();

	//Auto-crop all frames
	for(int i = 0; i < m_frames.size(); i++)
	{
		const SpriteSheetFrame& frame = m_frames[i];
		ion::Vector2i topLeft(ion::maths::S32_MAX, ion::maths::S32_MAX);
		ion::Vector2i bottomRight(ion::maths::S32_MIN, ion::maths::S32_MIN);

		//Find outmost non-0 pixels
		for(int tileX = 0; tileX < m_widthTiles; tileX++)
		{
			for(int tileY = 0; tileY < m_heightTiles; tileY++)
			{
				//Column major
				const Tile& tile = frame[(tileX * m_heightTiles) + tileY];

				for(int pixelX = 0; pixelX < tileWidth; pixelX++)
				{
					for(int pixelY = 0; pixelY < tileHeight; pixelY++)
					{
						u8 pixel = tile.GetPixelColour(pixelX, pixelY);

						int x = (tileX * tileWidth) + pixelX;
						int y = (tileY * tileHeight) + pixelY;

						if(pixel != 0)
						{
							if(x < topLeft.x)
								topLeft.x = x;
							if(y < topLeft.y)
								topLeft.y = y;
							if(x > bottomRight.x)
								bottomRight.x = x;
							if(y > bottomRight.y)
								bottomRight.y = y;
						}
					}
				}
			}
		}

		//If no pixels in image, use whole size
		if(topLeft.x > bottomRight.x || topLeft.y > bottomRight.y)
		{
			topLeft.x = 0;
			topLeft.y = 0;
			bottomRight.x = m_widthTiles * tileWidth;
			bottomRight.y = m_heightTiles * tileHeight;
		}

		//Clamp to tile size
		topLeft.x = ion::maths::RoundDownToNearest(topLeft.x, tileWidth);
		topLeft.y = ion::maths::RoundDownToNearest(topLeft.y, tileHeight);
		bottomRight.x = ion::maths::RoundUpToNearest(bottomRight.x, tileWidth);
		bottomRight.y = ion::maths::RoundUpToNearest(bottomRight.y, tileHeight);

		// Add size
		SpriteFrameDimensions dimensions;
		dimensions.topLeft = topLeft;
		dimensions.bottomRight = bottomRight;
		m_croppedDimensionData.push_back(dimensions);
	}
}

void SpriteSheet::Serialise(ion::io::Archive& archive)
{
	archive.Serialise(m_name, "name");
	archive.Serialise(m_frames, "frames");
	archive.Serialise(m_croppedDimensionData, "croppedDimensionData");
	archive.Serialise(m_animations, "animations");
	archive.Serialise(m_palette, "palette");
	archive.Serialise(m_widthTiles, "width");
	archive.Serialise(m_heightTiles, "height");

	if(m_croppedDimensionData.size() == 0)
	{
		CropAllFrames(8, 8);
	}
}

void SpriteSheet::ExportSpriteTiles(const PlatformConfig& config, std::stringstream& stream, const std::string& actorName) const
{
	int frameIdx = 0;
	for(std::vector<SpriteSheetFrame>::const_iterator it = m_frames.begin(), end = m_frames.end(); it != end; ++it, ++frameIdx)
	{
		//Split into subsprites
		const SpriteFrameDimensions& dimensions = GetDimensionData(frameIdx);
		const int widthTiles = GetWidthTiles(frameIdx, config.tileWidth);
		const int heightTiles = GetHeightTiles(frameIdx, config.tileHeight);
		const int leftTile = dimensions.topLeft.x / config.tileWidth;
		const int topTile = dimensions.topLeft.y / config.tileHeight;
		const int rightTile = dimensions.bottomRight.x / config.tileWidth;
		const int bottomTile = dimensions.bottomRight.y / config.tileHeight;

		const int widthSubSprites = ion::maths::Ceil((float)widthTiles / (float)s_subSpriteWidthTiles);
		const int heightSubSprites = ion::maths::Ceil((float)heightTiles / (float)s_subSpriteHeightTiles);

		//Sort into subsprites
		std::vector<std::vector<int>> subsprites(widthSubSprites * heightSubSprites, std::vector<int>());

		for(int x = leftTile; x < rightTile; x++)
		{
			for(int y = topTile; y < bottomTile; y++)
			{
				int subspriteX = (x - leftTile) / s_subSpriteWidthTiles;
				int subspriteY = (y - topTile) / s_subSpriteHeightTiles;

				//Column order
				int subspriteIdx = (subspriteX * heightSubSprites) + subspriteY;
				std::vector<int>& subsprite = subsprites[subspriteIdx];

				int subspriteWidth = ion::maths::Min(s_subSpriteWidthTiles, widthTiles - (s_subSpriteWidthTiles * subspriteX));
				int subspriteHeight = ion::maths::Min(s_subSpriteHeightTiles, heightTiles - (s_subSpriteHeightTiles * subspriteY));

				if(subsprite.size() == 0)
				{
					subsprite = std::vector<int>(subspriteWidth * subspriteHeight, 0);
				}

				int subspriteTileX = (x - leftTile) % s_subSpriteWidthTiles;
				int subspriteTileY = (y - topTile) % s_subSpriteHeightTiles;

				//Column order
				int subspriteTileIdx = (subspriteTileX * subspriteHeight) + subspriteTileY;
				subsprite[subspriteTileIdx] = (x * m_heightTiles) + y;
			}
		}

		std::stringstream label;
		label << "actor_" << actorName << "_sheet_" << m_name << "_frame_" << frameIdx;

		stream << label.str() << ":" << std::endl;

		//Export in subsprite order
		for(int i = 0; i < subsprites.size(); i++)
		{
			for(int j = 0; j < subsprites[i].size(); j++)
			{
				(*it)[subsprites[i][j]].Export(config, stream);
				stream << std::endl;
			}
		}

		stream << std::endl;
	}
}

void SpriteSheet::ExportSpriteTiles(const PlatformConfig& config, ion::io::File& file, const std::string& actorName) const
{

}

void SpriteSheet::ExportStampTiles(const PlatformConfig& config, const Stamp& referenceStamp, std::stringstream& stream, const std::string& actorName) const
{
#if defined _DEBUG
	ion::debug::Assert(referenceStamp.CheckTilesBatched(), "SpriteSheet::ExportStampTiles() - Tiles not in sequential order");
#endif

	//Get all unique tiles
	std::vector<TileId> tileIndices;
	int numUniqueTiles = referenceStamp.GetSortedUniqueTileBatch(tileIndices);

	//Offset by first index
	int firstIndex = tileIndices[0];

	int frameIdx = 0;
	for(std::vector<SpriteSheetFrame>::const_iterator it = m_frames.begin(), end = m_frames.end(); it != end; ++it, ++frameIdx)
	{
		std::stringstream label;
		label << "actor_" << actorName << "_sheet_" << m_name << "_frame_" << frameIdx;
		stream << label.str() << ":" << std::endl;

		//Export in sorted unique order
		for(int i = 0; i < tileIndices.size(); i++)
		{
			//Find index of first use of this tile id in reference stamp
			int tileIndex = referenceStamp.GetTileIndex(tileIndices[i]);

			//Transpose to sprite (column major) order
			ion::Vector2i pos(tileIndex / referenceStamp.GetWidth(), tileIndex % referenceStamp.GetWidth());
			int rowMajorIndex = (pos.y * referenceStamp.GetHeight()) + pos.x;

			//Export tile of animation frame
			(*it)[rowMajorIndex].Export(config, stream);

			stream << std::endl;
		}
	}
}

void SpriteSheet::ExportStampTiles(const PlatformConfig& config, const Stamp& referenceStamp, ion::io::File& file, const std::string& actorName) const
{

}

void SpriteSheet::ExportAnims(const PlatformConfig& config, std::stringstream& stream, const std::string& actorName) const
{
#if defined BEEHIVE_PLUGIN_LUMINARY
	stream << std::hex << std::setfill('0') << std::uppercase;

	for(TSpriteAnimMap::const_iterator it = m_animations.begin(), end = m_animations.end(); it != end; ++it)
	{
		std::stringstream label;
		label << "actor_" << actorName << "_sheet_" << m_name << "_anim_" << it->second.GetName();

		int numKeyframes = it->second.m_trackSpriteFrame.GetNumKeyframes();

		if(numKeyframes > 0)
		{
			//1 Mega Drive anim frame == 0x0100 subframes (at 50fps)
			int framesPerSecond = (int)it->second.GetPlaybackSpeed();
			int speed = (framesPerSecond << 8) / 50;
			int speedSubframes = speed << 8;

			bool looping = (it->second.GetPlaybackBehaviour() == ion::render::Animation::PlaybackBehaviour::Loop);

			stream << label.str() << ":" << std::endl;

			// SpriteAnim_KeyframeTrackFrameId         rs.l 1; Frame ID keyframe track
			// SpriteAnim_Length                       rs.w 1; Length in keyframes
			// SpriteAnim_DefaultSpeed                 rs.w 1; Default speed
			// SpriteAnim_DefaultLoop                  rs.b 1; Default loop flag

			stream << "\tdc.l " << label.str() << "_track_frames\t; Frame ID keyframe track" << std::endl;
			stream << "\tdc.w 0x" << HEX4(numKeyframes) << "\t; Num keyframes" << std::endl;
			stream << "\tdc.w 0x" << HEX4(speedSubframes) << "\t; Default speed" << std::endl;
			stream << "\tdc.b 0x" << (looping ? "1" : "0") << "\t; Default loop flag" << std::endl;
			stream << "\teven" << std::endl;
			stream << std::endl;

			stream << label.str() << "_track_frames:" << std::endl;
			it->second.m_trackSpriteFrame.Export(stream, actorName, m_name);
			stream << "\teven" << std::endl;

			if(it->second.m_trackPosition.GetNumKeyframes() > 0)
			{
				stream << label.str() << "_track_posx:" << std::endl;
				it->second.m_trackPosition.ExportX(stream, numKeyframes);
				stream << "\teven" << std::endl;
			}

			if(it->second.m_trackPosition.GetNumKeyframes() > 0)
			{
				stream << label.str() << "_track_posy:" << std::endl;
				it->second.m_trackPosition.ExportY(stream, numKeyframes);
				stream << "\teven" << std::endl;
			}
			
			if(it->second.m_trackSFX.GetNumKeyframes() > 0)
			{
				stream << label.str() << "_track_sfx:" << std::endl;
				it->second.m_trackSFX.Export(stream, numKeyframes);
				stream << "\teven" << std::endl;
			}
		}
	}

	stream << std::dec;

#else

	stream << std::hex << std::setfill('0') << std::uppercase;

	for (TSpriteAnimMap::const_iterator it = m_animations.begin(), end = m_animations.end(); it != end; ++it)
	{
		std::stringstream label;
		label << "actor_" << actorName << "_sheet_" << m_name << "_anim_" << it->second.GetName();

		int numKeyframes = it->second.m_trackSpriteFrame.GetNumKeyframes();

		if (numKeyframes > 0)
		{
			//1 Mega Drive anim frame == 0x0100 subframes (at 50fps)
			int framesPerSecond = (int)it->second.GetPlaybackSpeed();
			int speed = (framesPerSecond << 8) / 50;

			stream << label.str() << ":" << std::endl;
			stream << label.str() << "_numframes: equ 0x" << numKeyframes << std::endl;
			stream << label.str() << "_speed: equ 0x" << speed << std::endl;

			stream << label.str() << "_track_frames:" << std::endl;
			it->second.m_trackSpriteFrame.Export(stream, actorName, m_name);
			stream << "\tEven" << std::endl;

			if (it->second.m_trackPosition.GetNumKeyframes() > 0)
			{
				stream << label.str() << "_track_posx:" << std::endl;
				it->second.m_trackPosition.ExportX(stream, numKeyframes);
				stream << "\tEven" << std::endl;
			}
			else
			{
				stream << label.str() << "_track_posx equ 0" << std::endl;
			}

			if (it->second.m_trackPosition.GetNumKeyframes() > 0)
			{
				stream << label.str() << "_track_posy:" << std::endl;
				it->second.m_trackPosition.ExportY(stream, numKeyframes);
				stream << "\tEven" << std::endl;
			}
			else
			{
				stream << label.str() << "_track_posy equ 0" << std::endl;
			}

			if (it->second.m_trackSFX.GetNumKeyframes() > 0)
			{
				stream << label.str() << "_track_sfx:" << std::endl;
				it->second.m_trackSFX.Export(stream, numKeyframes);
				stream << "\tEven" << std::endl;
			}
			else
			{
				stream << label.str() << "_track_sfx equ 0" << std::endl;
			}
		}
	}

	stream << std::dec;

#endif // BEEHIVE_PLUGIN_LUMINARY
}

void SpriteSheet::ExportAnims(const PlatformConfig& config, ion::io::File& file, const std::string& actorName) const
{

}

void SpriteSheet::ExportPalette(const PlatformConfig& config, std::stringstream& stream) const
{
	m_palette.Export(stream);
}

void SpriteSheet::ExportSpriteFrameDimensions(const PlatformConfig& config, std::stringstream& stream, const std::string& actorName, int sizeUniqueTiles) const
{
	for(int i = 0; i < GetNumFrames(); i++)
	{
		std::stringstream label;
		label << "actor_" << actorName << "_sheet_" << m_name << "_frame_" << i;

		u8 widthTotal;
		u8 widthWhole;
		u8 widthRemainder;
		u8 heightTotal;
		u8 heightWhole;
		u8 heightRemainder;

		std::vector<ion::Vector2i> subSprDimensionsTiles;
		std::vector<ion::Vector2i> subSprOffsetsUnflipped;
		std::vector<ion::Vector2i> subSprOffsetsFlippedX;
		std::vector<ion::Vector2i> subSprOffsetsFlippedY;
		std::vector<ion::Vector2i> subSprOffsetsFlippedXY;

		GetWidthSubsprites(i, config.tileWidth, widthTotal, widthWhole, widthRemainder);
		GetHeightSubsprites(i, config.tileHeight, heightTotal, heightWhole, heightRemainder);
		GetSubspriteDimensions(i, subSprDimensionsTiles, config.tileWidth, config.tileHeight);
		GetSubspritePosOffsets(i, subSprOffsetsUnflipped, config.tileWidth, config.tileHeight);
		GetSubspritePosOffsetsFlippedX(i, subSprOffsetsFlippedX, config.tileWidth, config.tileHeight);
		GetSubspritePosOffsetsFlippedY(i, subSprOffsetsFlippedY, config.tileWidth, config.tileHeight);
		GetSubspritePosOffsetsFlippedXY(i, subSprOffsetsFlippedXY, config.tileWidth, config.tileHeight);

		int sizeTiles = sizeUniqueTiles;

		if(sizeUniqueTiles == 0)
		{
			sizeTiles = GetWidthTiles(i, config.tileWidth) * GetHeightTiles(i, config.tileHeight);
		}

		stream << label.str() << "_size_b\t\tequ 0x" << HEX4(sizeTiles * 32) << "\t; Size in bytes" << std::endl;
		stream << label.str() << "_size_t\t\tequ 0x" << HEX4(sizeTiles) << "\t; Size in tiles" << std::endl;
		stream << label.str() << "_size_subsprites\t\tequ 0x" << HEX4(widthTotal * heightTotal) << "\t; Size in subsprites" << std::endl;

		stream << std::endl;

		stream << "; Subsprite offsets (deltas) (in pixels) - unflipped (ww), flipped X (ww), flipped Y (ww), flipped XY (ww)" << std::endl;
		stream << label.str() << "_subsprite_pos_offsets:" << std::endl;

		for(int i = 0; i < subSprOffsetsUnflipped.size(); i++)
		{
			stream << "\tdc.l 0x"
				<< HEX4(subSprOffsetsUnflipped[i].x)
				<< HEX4(subSprOffsetsUnflipped[i].y)
				<< ", 0x"
				<< HEX4(subSprOffsetsFlippedX[i].x)
				<< HEX4(subSprOffsetsFlippedX[i].y)
#if defined BEEHIVE_PLUGIN_LUMINARY
				<< ", 0x"
				<< HEX4(subSprOffsetsFlippedY[i].x)
				<< HEX4(subSprOffsetsFlippedY[i].y)
				<< ", 0x"
				<< HEX4(subSprOffsetsFlippedXY[i].x)
				<< HEX4(subSprOffsetsFlippedXY[i].y)
#endif
				<< std::endl;
		}

		stream << "\tEven" << std::endl << std::endl;

		stream << "; Subsprite dimension bits (for sprite descs)" << std::endl;
		stream << label.str() << "_subsprite_dimensions_bits:" << std::endl;

		for(int i = 0; i < subSprDimensionsTiles.size(); i++)
		{
			u8 bits = ((subSprDimensionsTiles[i].x - 1) << 2) | (subSprDimensionsTiles[i].y - 1);
			stream << "\tdc.b 0x" << HEX1(bits) << std::endl;
		}

		//To avoid source control diff noise
#if defined BEEHIVE_PLUGIN_LUMINARY
		stream << "\teven" << std::endl << std::endl;
#else
		stream << "\tEven" << std::endl << std::endl;
#endif
	}

	stream << std::dec;
	stream << std::endl;
}

void SpriteSheet::ExportSpriteFrameOffsetsTable(const PlatformConfig& config, std::stringstream& stream, const std::string& actorName, int sizeUniqueTiles) const
{
#if defined BEEHIVE_PLUGIN_LUMINARY
	// (w) Frame size (tiles)
	// (w) Frame size (subsprites)
	// (l) Tiles address
	// (l) Subsprite dimension bits table addr
	// (l) Subsprite position offsets table addr

	for(int i = 0; i < GetNumFrames(); i++)
	{
		std::stringstream label;
		label << "actor_" << actorName << "_sheet_" << m_name << "_frame_" << i;

		stream << label.str() << "_header:" << std::endl;

		const SpriteSheet::SpriteFrameDimensions& dimensionData = GetDimensionData(i);

		int sizeTiles = sizeUniqueTiles;

		if(sizeUniqueTiles == 0)
		{
			sizeTiles = GetWidthTiles(i, config.tileWidth) * GetHeightTiles(i, config.tileHeight);
		}

		stream << "\tdc.w 0x" << HEX4(sizeTiles) << "\t; SpriteFrame_SizeTiles" << std::endl;
		stream << "\tdc.w " << label.str() << "_size_subsprites\t; SpriteFrame_SizeSubsprites" << std::endl;
		stream << "\tdc.l " << label.str() << "\t; SpriteFrame_TileData" << std::endl;
		stream << "\tdc.l " << label.str() << "_subsprite_dimensions_bits" << "\t; SpriteFrame_LayoutTable" << std::endl;
		stream << "\tdc.l " << label.str() << "_subsprite_pos_offsets" << "\t; SpriteFrame_PosOffsetTable" << std::endl;
		stream << std::endl;
	}

	stream << "actor_" << actorName << "_sheet_" << m_name << "_frametable:" << std::endl;

#else // BEEHIVE_PLUGIN_LUMINARY

	stream << "actor_" << actorName << "_sheet_" << m_name << "_frametable:" << std::endl;

	// (w) Frame size (tiles)
	// (w) Frame size (subsprites)
	// (l) Tiles address
	// (l) Subsprite dimension bits table addr
	// (l) Subsprite position offsets table addr

	for (int i = 0; i < GetNumFrames(); i++)
	{
		std::stringstream label;
		label << "actor_" << actorName << "_sheet_" << m_name << "_frame_" << i;

		stream << label.str() << "_header:" << std::endl;

		const SpriteSheet::SpriteFrameDimensions& dimensionData = GetDimensionData(i);

		int sizeTiles = sizeUniqueTiles;

		if (sizeUniqueTiles == 0)
		{
			sizeTiles = GetWidthTiles(i, config.tileWidth) * GetHeightTiles(i, config.tileHeight);
		}

		stream << "\tdc.w 0x" << HEX4(sizeTiles) << "\t; Frame size (tiles)" << std::endl;
		stream << "\tdc.w " << label.str() << "_size_subsprites\t; Frame size (subsprites)" << std::endl;
		stream << "\tdc.l " << label.str() << std::endl;
		stream << "\tdc.l " << label.str() << "_subsprite_dimensions_bits" << std::endl;
		stream << "\tdc.l " << label.str() << "_subsprite_pos_offsets" << std::endl;
		stream << std::endl;
	}

#endif
}

u32 SpriteSheet::GetBinarySizeTiles() const
{
	u32 size = 0;

	for(std::vector<SpriteSheetFrame>::const_iterator it = m_frames.begin(), end = m_frames.end(); it != end; ++it)
	{
		for(int i = 0; i < it->size(); i++)
		{
			size += (*it)[i].GetBinarySize();
		}
	}

	return size;
}
