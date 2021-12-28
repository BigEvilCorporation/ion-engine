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

#pragma once

#include "Tile.h"
#include "Palette.h"
#include "SpriteAnimation.h"

#include <ion/core/cryptography/UUID.h>

typedef std::vector<Tile> SpriteSheetFrame;

class Stamp;

class SpriteSheet
{
public:
	static const int s_subSpriteWidthTiles;
	static const int s_subSpriteHeightTiles;

	struct SpriteFrameDimensions
	{
		ion::Vector2i sizeSubsprites;
		ion::Vector2i topLeft;
		ion::Vector2i bottomRight;
		std::vector<ion::Vector2i> subspriteDimensions;
		std::vector<ion::Vector2i> subspriteOffsets;
		std::vector<ion::Vector2i> subspriteOffsetsFlippedX;

		void Serialise(ion::io::Archive& archive)
		{
			archive.Serialise(sizeSubsprites, "sizeSubsprites");
			archive.Serialise(topLeft, "topLeft");
			archive.Serialise(bottomRight, "bottomRight");
			archive.Serialise(subspriteDimensions, "subspriteDimensions");
			archive.Serialise(subspriteOffsets, "subspriteOffsets");
			archive.Serialise(subspriteOffsetsFlippedX, "subspriteOffsetsFlippedX");
		}
	};

	SpriteSheet();

	bool ImportBitmap(const std::string& filename, const std::string& name, int tileWidth, int tileHeight, int widthFrames, int heightFrames, int startFrame, int endFrame);

	const std::string& GetName() const;
	void SetName(const std::string& name);

	const SpriteSheetFrame& GetFrame(int index) const;
	int GetNumFrames() const;

	int GetNumCroppedTiles(int tileWidth, int tileHeight) const;

	SpriteAnimId AddAnimation();
	void DeleteAnimation(SpriteAnimId animId);
	SpriteAnimation* GetAnimation(SpriteAnimId animId);
	const SpriteAnimation* GetAnimation(SpriteAnimId animId) const;
	SpriteAnimation* FindAnimation(const std::string name);
	const SpriteAnimation* FindAnimation(const std::string name) const;
	AnimationId FindAnimationId(const std::string name) const;
	int GetNumAnimations() const;
	TSpriteAnimMap::const_iterator AnimationsBegin() const;
	TSpriteAnimMap::const_iterator AnimationsEnd() const;
	TSpriteAnimMap::iterator AnimationsBegin();
	TSpriteAnimMap::iterator AnimationsEnd();

	u8 GetWidthTiles() const;
	u8 GetHeightTiles() const;
	u8 GetWidthTiles(int frameIdx, int tileWidth) const;
	u8 GetHeightTiles(int frameIdx, int tileHeight) const;
	u8 GetMaxWidthTiles(int tileWidth) const;
	u8 GetMaxHeightTiles(int tileHeight) const;

	const SpriteFrameDimensions& GetDimensionData(int frameIdx) const;

	const Palette& GetPalette() const;

	//Crop all frames and regenerate dimension data
	void CropAllFrames(int tileWidth, int tileHeight);

	//Clear all frames
	void ClearFrames() { m_frames.clear(); }

	//Serialise
	void Serialise(ion::io::Archive& archive);
	void ExportSpriteTiles(const PlatformConfig& config, std::stringstream& stream, const std::string& actorName) const;
	void ExportSpriteTiles(const PlatformConfig& config, ion::io::File& file, const std::string& actorName) const;
	void ExportStampTiles(const PlatformConfig& config, const Stamp& referenceStamp, std::stringstream& stream, const std::string& actorName) const;
	void ExportStampTiles(const PlatformConfig& config, const Stamp& referenceStamp, ion::io::File& file, const std::string& actorName) const;
	void ExportAnims(const PlatformConfig& config, std::stringstream& stream, const std::string& actorName) const;
	void ExportAnims(const PlatformConfig& config, ion::io::File& file, const std::string& actorName) const;
	void ExportPalette(const PlatformConfig& config, std::stringstream& stream) const;
	void ExportSpriteFrameDimensions(const PlatformConfig& config, std::stringstream& stream, const std::string& actorName, int sizeUniqueTiles = 0) const;
	void ExportSpriteFrameOffsetsTable(const PlatformConfig& config, std::stringstream& stream, const std::string& actorName, int sizeUniqueTiles = 0) const;
	u32 GetBinarySizeTiles() const;

	void GetWidthSubsprites(int frameIdx, int tileWidth, u8& total, u8& whole, u8& remainder) const;
	void GetHeightSubsprites(int frameIdx, int tileHeight, u8& total, u8& whole, u8& remainder) const;

	void GetSubspriteDimensions(u32 frameIdx, std::vector<ion::Vector2i>& dimensions, int tileWidth, int tileHeight) const;

	void GetSubspritePosOffsets(u32 frameIdx, std::vector<ion::Vector2i>& offsets, int tileWidth, int tileHeight) const;
	void GetSubspritePosOffsetsFlippedX(u32 frameIdx, std::vector<ion::Vector2i>& offsets, int tileWidth, int tileHeight) const;
	void GetSubspritePosOffsetsFlippedY(u32 frameIdx, std::vector<ion::Vector2i>& offsets, int tileWidth, int tileHeight) const;
	void GetSubspritePosOffsetsFlippedXY(u32 frameIdx, std::vector<ion::Vector2i>& offsets, int tileWidth, int tileHeight) const;

private:
	std::string m_name;
	std::vector<SpriteSheetFrame> m_frames;
	std::vector<SpriteFrameDimensions> m_croppedDimensionData;
	TSpriteAnimMap m_animations;
	Palette m_palette;
	u8 m_widthTiles;
	u8 m_heightTiles;
};
