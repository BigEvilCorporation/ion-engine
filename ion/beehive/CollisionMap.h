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

#include <vector>
#include <sstream>

#include <ion/core/io/Archive.h>
#include <maths/Vector.h>

#include <ion/gamekit/Bezier.h>

#include "TerrainTile.h"
#include "PlatformConfig.h"

class Project;

class CollisionMap
{
public:

	static const u16 s_collisionTileFlagMask = eCollisionTileFlagAll;
	static const u16 s_collisionTileTerrainIdMask = eCollisionTileFlagNone;

	static const int defaultWidth = 64;
	static const int defaultHeight = 64;

	typedef u16 TTerrainTileDesc;

	struct Block
	{
		std::vector<TTerrainTileDesc> m_tiles;
		int uniqueIndex = -1;

		bool operator ==(const Block& rhs) const;
		void Export(const Project& project, std::stringstream& stream, int blockWidth, int blockHeight);
		void Export(const Project& project, ion::io::File& file, int blockWidth, int blockHeight);
	};

	CollisionMap();
	CollisionMap(const CollisionMap& rhs);
	CollisionMap(const PlatformConfig& platformConfig);
	CollisionMap(const PlatformConfig& platformConfig, int width, int height);

	void Clear();

	int GetWidth() const;
	int GetHeight() const;

	int GetBlockAlignedWidth(int blockWidth) const;
	int GetBlockAlignedHeight(int blockHeight) const;

	int GetWidthBlocks(int blockWidth) const;
	int GetHeightBlocks(int blockHeight) const;

	void Resize(int w, int h, bool shiftRight, bool shiftDown);

	//Set collision tile on CollisionMap
	inline void SetTerrainTile(int x, int y, TerrainTileId tile);
	inline TerrainTileId GetTerrainTile(int x, int y) const;

	//Set collision tile flags
	inline void SetCollisionTileFlags(int x, int y, u16 flags);
	inline u16 GetCollisionTileFlags(int x, int y) const;

	//Collision beziers
	ion::gamekit::BezierPath* AddTerrainBezier();
	void AddTerrainBezier(const ion::gamekit::BezierPath& bezier);
	ion::gamekit::BezierPath* GetTerrainBezier(u32 index);
	const ion::gamekit::BezierPath* GetTerrainBezier(u32 index) const;
	const ion::gamekit::BezierPath* FindTerrainBezier(int x, int y, ion::Vector2i& topLeft) const;
	int FindTerrainBeziers(int x, int y, int width, int height, std::vector<const ion::gamekit::BezierPath*>& beziers) const;
	void SetTerrainBezierFlags(u32 index, u16 flags);
	u16 GetTerrainBezierFlags(u32 index) const;
	void SetTerrainBezierLayer(u32 index, u8 layer);
	u8 GetTerrainBezierLayer(u32 index) const;
	void SetTerrainBezierGenerateWidth(u32 index, bool generateWidth);
	bool GetTerrainBezierGenerateWidth(u32 index) const;
	void RemoveTerrainBezier(u32 index);
	int GetNumTerrainBeziers() const;

	//Generate NxN blocks and sort unique
	void GenerateBlocks(const Project& project, int blockWidth, int blockHeight);
	std::vector<Block>& GetBlocks();

	//Get populated collision area
	void CalculatePhysicsWorldBounds(ion::Vector2i& topLeft, ion::Vector2i& size, int tileWidth, int tileHeight, int blockWidth, int blockHeight);
	void CalculatePhysicsWorldBoundsBlocks(ion::Vector2i& topLeft, ion::Vector2i& size, int tileWidth, int tileHeight, int blockWidth, int blockHeight);
	void GetPhysicsWorldBounds(ion::Vector2i& topLeft, ion::Vector2i& size) const { topLeft = m_physicsWorldTopLeft; size = m_physicsWorldSize; }
	void GetPhysicsWorldBoundsBlocks(ion::Vector2i& topLeft, ion::Vector2i& size) const { topLeft = m_physicsWorldTopLeftBlocks; size = m_physicsWorldSizeBlocks; }

	void Serialise(ion::io::Archive& archive);
	void Export(const Project& project, std::stringstream& stream) const;
	void Export(const Project& project, ion::io::File& file) const;
	void ExportBlockMap(const Project& project, std::stringstream& stream, int blockWidth, int blockHeight) const;
	void ExportBlockMap(const Project& project, ion::io::File& file, int blockWidth, int blockHeight, std::vector<u16>& rowOffsets) const;

private:

	struct TerrainBezier
	{
		ion::gamekit::BezierPath bezier;
		u16 terrainFlags;
		u8 layer;
		bool generateWidthData;

		void Serialise(ion::io::Archive& archive)
		{
			archive.Serialise(bezier, "bezier");
			archive.Serialise(terrainFlags, "flags");
			archive.Serialise(layer, "layer");
			archive.Serialise(generateWidthData, "generateWidthData");
		}
	};

	int m_width;
	int m_height;

	ion::Vector2i m_physicsWorldTopLeft;
	ion::Vector2i m_physicsWorldSize;
	ion::Vector2i m_physicsWorldTopLeftBlocks;
	ion::Vector2i m_physicsWorldSizeBlocks;

	//Tiles
	std::vector<TerrainTileId> m_collisionTiles;

	//Blocks
	std::vector<Block> m_blocks;

	//Terrain beziers
	std::vector<TerrainBezier> m_terrainBeziers;
};

void CollisionMap::SetTerrainTile(int x, int y, TerrainTileId tile)
{
	ion::debug::Assert(tile <= eCollisionTileFlagNone, "CollisionMap::SetTerrainTile() - Collision tile ID out of range");
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