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

#include "CollisionMap.h"
#include "Project.h"
#include <algorithm>

#include <ion/core/memory/Memory.h>
#include <ion/core/memory/Endian.h>
#include <ion/maths/Geometry.h>

CollisionMap::CollisionMap()
{
	const PlatformConfig& defaultPlatformConfig = PlatformPresets::s_configs[PlatformPresets::ePresetMegaDrive];
	m_width = 0;
	m_height = 0;
	Resize(defaultPlatformConfig.scrollPlaneWidthTiles, defaultPlatformConfig.scrollPlaneHeightTiles, false, false);
}

CollisionMap::CollisionMap(const PlatformConfig& platformConfig)
{
	m_width = 0;
	m_height = 0;
	Resize(platformConfig.scrollPlaneWidthTiles, platformConfig.scrollPlaneHeightTiles, false, false);
}

CollisionMap::CollisionMap(const PlatformConfig& platformConfig, int width, int height)
{
	m_width = 0;
	m_height = 0;
	Resize(width, height, false, false);
}

void CollisionMap::Clear()
{
	//Clear tiles
	for(int x = 0; x < m_width; x++)
	{
		for(int y = 0; y < m_height; y++)
		{
			SetTerrainTile(x, y, InvalidTerrainTileId);
		}
	}
}

void CollisionMap::Serialise(ion::io::Archive& archive)
{
	archive.Serialise(m_width, "width");
	archive.Serialise(m_height, "height");
	archive.Serialise(m_terrainBeziers, "terrainBeziers");
	archive.Serialise(m_collisionTiles, "collisionTiles");
}

int CollisionMap::GetWidth() const
{
	return m_width;
}

int CollisionMap::GetHeight() const
{
	return m_height;
}

int CollisionMap::GetBlockAlignedWidth(int blockWidth) const
{
	return ion::maths::RoundUpToNearest((int)m_width, blockWidth);
}

int CollisionMap::GetBlockAlignedHeight(int blockHeight) const
{
	return ion::maths::RoundUpToNearest((int)m_height, blockHeight);
}

int CollisionMap::GetWidthBlocks(int blockWidth) const
{
	return GetBlockAlignedWidth(blockWidth) / blockWidth;
}

int CollisionMap::GetHeightBlocks(int blockHeight) const
{
	return GetBlockAlignedHeight(blockHeight) / blockHeight;
}

void CollisionMap::Resize(int width, int height, bool shiftRight, bool shiftDown)
{
	//Create new tile array
	std::vector<TerrainTileId> terrainTiles;

	//Set new size
	int size = width * height;
	terrainTiles.resize(size);

	//Fill with invalid tile
	std::fill(terrainTiles.begin(), terrainTiles.end(), InvalidTerrainTileId);

	//Copy tiles
	for(int x = 0; x < ion::maths::Min(width, m_width); x++)
	{
		for(int y = 0; y < ion::maths::Min(height, m_height); y++)
		{
			int destTileIdx = (y * width) + x;
			if(shiftRight && width > m_width)
				destTileIdx += (width - m_width);
			if(shiftDown && height > m_height)
				destTileIdx += (height - m_height) * width;

			terrainTiles[destTileIdx] = GetTerrainTile(x, y);
		}
	}
	
	//Set new
	m_collisionTiles = terrainTiles;
	m_width = width;
	m_height = height;
}

void CollisionMap::SetTerrainTile(int x, int y, TerrainTileId tile)
{
	u32 tileIdx = (y * m_width) + x;
	ion::debug::Assert(tileIdx < (m_width * m_height), "CollisionMap::SetTerrainTile() - Out of range");
	u32& entry = m_collisionTiles[tileIdx];
	entry &= ~s_collisionTileTerrainIdMask;
	entry |= tile;
}

TerrainTileId CollisionMap::GetTerrainTile(int x, int y) const
{
	u32 tileIdx = (y * m_width) + x;
	ion::debug::Assert(tileIdx < (m_width * m_height), "CollisionMap::GetTerrainTile() - Out of range");
	return m_collisionTiles[tileIdx] & s_collisionTileTerrainIdMask;
}

void CollisionMap::SetCollisionTileFlags(int x, int y, u16 flags)
{
	int tileIdx = (y * m_width) + x;
	ion::debug::Assert(tileIdx < (m_width * m_height), "CollisionMap::GetTerrainTile() - Out of range");
	u32& entry = m_collisionTiles[tileIdx];
	entry &= ~s_collisionTileFlagMask;
	entry |= flags;
}

u16 CollisionMap::GetCollisionTileFlags(int x, int y) const
{
	int tileIdx = (y * m_width) + x;
	ion::debug::Assert(tileIdx < (m_width * m_height), "CollisionMap::GetCollisionTileFlags() - Out of range");
	return m_collisionTiles[tileIdx] & s_collisionTileFlagMask;
}

ion::gamekit::BezierPath* CollisionMap::AddTerrainBezier()
{
	m_terrainBeziers.push_back(ion::gamekit::BezierPath());
	return &m_terrainBeziers.back();
}

void CollisionMap::AddTerrainBezier(const ion::gamekit::BezierPath& bezier)
{
	m_terrainBeziers.push_back(bezier);
}

ion::gamekit::BezierPath* CollisionMap::GetTerrainBezier(u32 index)
{
	ion::debug::Assert(index < m_terrainBeziers.size(), "CollisionMap::GetTerrainBezier() - Out of range");
	return &m_terrainBeziers[index];
}

const ion::gamekit::BezierPath* CollisionMap::GetTerrainBezier(u32 index) const
{
	ion::debug::Assert(index < m_terrainBeziers.size(), "CollisionMap::GetTerrainBezier() - Out of range");
	return &m_terrainBeziers[index];
}

const ion::gamekit::BezierPath* CollisionMap::FindTerrainBezier(int x, int y, ion::Vector2i& topLeft) const
{
	const ion::gamekit::BezierPath* bezier = NULL;
	ion::Vector2i bottomRight;

	ion::Vector2 bezierBoundsMin;
	ion::Vector2 bezierBoundsMax;

	for(int i = 0; i < m_terrainBeziers.size() && !bezier; i++)
	{
		m_terrainBeziers[i].GetBounds(bezierBoundsMin, bezierBoundsMax);

		if(x >= topLeft.x && y >= topLeft.y
			&& x < bottomRight.x && y < bottomRight.y)
		{
			bezier = &m_terrainBeziers[i];
			topLeft.x = bezierBoundsMin.x;
			topLeft.y = bezierBoundsMin.y;
		}
	}

	return bezier;
}

int CollisionMap::FindTerrainBeziers(int x, int y, int width, int height, std::vector<const ion::gamekit::BezierPath*>& beziers) const
{
	ion::Vector2i boundsMin(x, y);
	ion::Vector2i boundsMax(x + width, y + height);

	ion::Vector2 bezierBoundsMin;
	ion::Vector2 bezierBoundsMax;

	for(int i = 0; i < m_terrainBeziers.size(); i++)
	{
		const ion::gamekit::BezierPath& bezier = m_terrainBeziers[i];
		bezier.GetBounds(bezierBoundsMin, bezierBoundsMax);
		if(ion::maths::BoxIntersectsBox(boundsMin, boundsMax, ion::Vector2i(bezierBoundsMin.x, bezierBoundsMin.y), ion::Vector2i(bezierBoundsMax.x, bezierBoundsMax.y)))
		{
			beziers.push_back(&bezier);
		}
	}

	return beziers.size();
}

void CollisionMap::RemoveTerrainBezier(u32 index)
{
	ion::debug::Assert(index < m_terrainBeziers.size(), "CollisionMap::RemoveTerrainBezier() - Out of range");
	m_terrainBeziers.erase(m_terrainBeziers.begin() + index);
}

int CollisionMap::GetNumTerrainBeziers() const
{
	return m_terrainBeziers.size();
}

void CollisionMap::GenerateBlocks(const Project& project, int blockWidth, int blockHeight)
{
	//Align map size to block size
	int mapWidth = GetBlockAlignedWidth(blockWidth);
	int mapHeight = GetBlockAlignedHeight(blockHeight);

	//Use default tile if there is one, else use first tile
	u32 defaultTileId = project.GetDefaultTerrainTile();
	if(defaultTileId == InvalidTerrainTileId)
	{
		defaultTileId = 0;
	}

	//Copy tiles (using new aligned map size)
	std::vector<TTerrainTileDesc> tiles;
	tiles.resize(mapWidth * mapHeight);
	std::fill(tiles.begin(), tiles.end(), defaultTileId);

	for(int x = 0; x < m_width; x++)
	{
		for(int y = 0; y < m_height; y++)
		{
			TerrainTileId tileId = GetTerrainTile(x, y);
			u16 tileFlags = GetCollisionTileFlags(x, y);

			if(tileId == InvalidTerrainTileId)
			{
				tileId = defaultTileId;
			}

			tiles[(y * mapWidth) + x] = (u16)tileId | tileFlags;
		}
	}

	//Split map into NxN blocks
	int widthBlocks = mapWidth / blockWidth;
	int heightBlocks = mapHeight / blockHeight;

	m_blocks.clear();
	m_blocks.reserve(widthBlocks * heightBlocks);

	for(int blockY = 0; blockY < heightBlocks; blockY++)
	{
		for(int blockX = 0; blockX < widthBlocks; blockX++)
		{
			m_blocks.push_back(Block());
			Block& block = m_blocks.back();

			int blockStartOffset = (blockY * widthBlocks * blockWidth) + (blockX * blockWidth);

			for(int tileY = 0; tileY < blockHeight; tileY++)
			{
				for(int tileX = 0; tileX < blockWidth; tileX++)
				{
					int x = (blockX * blockWidth) + tileX;
					int y = (blockY * blockHeight) + tileY;

					int tileOffset = (y * mapWidth) + x;
					block.m_tiles.push_back(tiles[tileOffset]);
				}
			}
		}
	}

	//Find duplicates
	m_uniqueBlocks.clear();

	for(int i = 0; i < m_blocks.size(); i++)
	{
		if(m_blocks[i].uniqueIndex == -1)
		{
			m_blocks[i].uniqueIndex = m_uniqueBlocks.size();
			m_uniqueBlocks.push_back(&m_blocks[i]);

			for(int j = i + 1; j < m_blocks.size(); j++)
			{
				if(m_blocks[i] == m_blocks[j])
				{
					m_blocks[j].uniqueIndex = m_blocks[i].uniqueIndex;
				}
			}
		}
	}
}

void CollisionMap::Export(const Project& project, std::stringstream& stream) const
{
	//Use default tile if there is one, else use first tile
	u32 defaultTileId = project.GetDefaultTerrainTile();
	if(defaultTileId == InvalidTerrainTileId)
	{
		defaultTileId = 0;
	}

	//Output to stream
	stream << std::hex << std::setfill('0') << std::uppercase;

	for(int y = 0; y < m_height; y++)
	{
		stream << "\tdc.w\t";

		for(int x = 0; x < m_width; x++)
		{
			TerrainTileId tileId = GetTerrainTile(x, y);
			u16 tileFlags = GetCollisionTileFlags(x, y);

			//If blank tile, use default tile
			u32 tileIndex = (tileId == InvalidTerrainTileId) ? defaultTileId : tileId;

			u16 word = (u16)tileIndex | tileFlags;
			stream << "0x" << std::setw(4) << (u32)word;

			if(x < (m_width - 1))
				stream << ",";
		}

		stream << std::endl;
	}

	stream << std::dec;
}

void CollisionMap::Export(const Project& project, ion::io::File& file) const
{
	//Use default tile if there is one, else use first tile
	u32 defaultTileId = project.GetDefaultTerrainTile();
	if(defaultTileId == InvalidTerrainTileId)
	{
		defaultTileId = 0;
	}

	for(int y = 0; y < m_height; y++)
	{
		for(int x = 0; x < m_width; x++)
		{
			TerrainTileId tileId = GetTerrainTile(x, y);
			u16 tileFlags = GetCollisionTileFlags(x, y);

			//If blank tile, use default tile
			u32 tileIndex = (tileId == InvalidTerrainTileId) ? defaultTileId : tileId;

			u16 word = (u16)tileIndex | tileFlags;
			ion::memory::EndianSwap(word);
			file.Write(&word, sizeof(u16));
		}
	}
}

void CollisionMap::ExportBlocks(const Project& project, std::stringstream& stream, int blockWidth, int blockHeight) const
{
	//Export unique blocks
	for(int i = 0; i < m_uniqueBlocks.size(); i++)
	{
		stream << "Terrain_Block_" << i << ":" << std::endl;
		m_uniqueBlocks[i]->Export(project, stream, blockWidth, blockHeight);
		stream << std::endl;
	}
}

void CollisionMap::ExportBlocks(const Project& project, ion::io::File& file, int blockWidth, int blockHeight) const
{
	//Export unique blocks
	for(int i = 0; i < m_uniqueBlocks.size(); i++)
	{
		m_uniqueBlocks[i]->Export(project, file, blockWidth, blockHeight);
	}
}

void CollisionMap::ExportBlockMap(const Project& project, std::stringstream& stream, int blockWidth, int blockHeight) const
{
	int widthBlocks = GetWidthBlocks(blockWidth);
	int heightBlocks = GetHeightBlocks(blockHeight);

	//Export block map
	stream << std::endl;
	stream << "Terrain_Block_Map:" << std::endl;

	for(int blockY = 0; blockY < heightBlocks; blockY++)
	{
		stream << "\tdc.w\t";

		for(int blockX = 0; blockX < widthBlocks; blockX++)
		{
			int blockId = (blockY * widthBlocks) + blockX;
			const Block& block = m_blocks[blockId];

			stream << "0x" << std::hex << std::setfill('0') << std::setw(4) << block.uniqueIndex;

			if(blockX < (widthBlocks - 1))
			{
				stream << ",";
			}
		}

		stream << std::endl;
	}
}

void CollisionMap::ExportBlockMap(const Project& project, ion::io::File& file, int blockWidth, int blockHeight) const
{
	int widthBlocks = GetWidthBlocks(blockWidth);
	int heightBlocks = GetHeightBlocks(blockHeight);

	//Export block map
	for(int blockY = 0; blockY < heightBlocks; blockY++)
	{
		for(int blockX = 0; blockX < widthBlocks; blockX++)
		{
			int blockId = (blockY * widthBlocks) + blockX;
			const Block& block = m_blocks[blockId];

			u16 word = block.uniqueIndex;

			//Endian flip
			ion::memory::EndianSwap(word);

			//Write
			file.Write(&word, sizeof(u16));
		}
	}
}

bool CollisionMap::Block::operator ==(const Block& rhs) const
{
	return m_tiles == rhs.m_tiles;
}

void CollisionMap::Block::Export(const Project& project, std::stringstream& stream, int blockWidth, int blockHeight)
{
	//Output to stream
	stream << std::hex << std::setfill('0') << std::uppercase;

	for(int y = 0; y < blockHeight; y++)
	{
		stream << "\tdc.w\t";

		for(int x = 0; x < blockWidth; x++)
		{
			const TTerrainTileDesc& tileDesc = m_tiles[(y * blockWidth) + x];

			//Generate word
			u16 word = tileDesc;

			//Write
			stream << "0x" << std::hex << std::setfill('0') << std::setw(4) << (u32)word;

			if(x < (blockWidth - 1))
			{
				stream << ",";
			}
		}

		stream << std::endl;
	}
}

void CollisionMap::Block::Export(const Project& project, ion::io::File& file, int blockWidth, int blockHeight)
{
	//Output to file
	for(int y = 0; y < blockHeight; y++)
	{
		for(int x = 0; x < blockWidth; x++)
		{
			const TTerrainTileDesc& tileDesc = m_tiles[(y * blockWidth) + x];

			//Generate word
			u16 word = tileDesc;

			//Endian flip
			ion::memory::EndianSwap(word);

			//Write
			file.Write(&word, sizeof(u16));
		}
	}
}