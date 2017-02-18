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

#include <io/Archive.h>
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

	CollisionMap();
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
	void SetTerrainTile(int x, int y, TerrainTileId tile);
	TerrainTileId GetTerrainTile(int x, int y) const;

	//Set collision tile flags
	void SetCollisionTileFlags(int x, int y, u16 flags);
	u16 GetCollisionTileFlags(int x, int y) const;

	//Collision beziers
	ion::gamekit::BezierPath* AddTerrainBezier();
	void AddTerrainBezier(const ion::gamekit::BezierPath& bezier);
	ion::gamekit::BezierPath* GetTerrainBezier(u32 index);
	const ion::gamekit::BezierPath* GetTerrainBezier(u32 index) const;
	const ion::gamekit::BezierPath* CollisionMap::FindTerrainBezier(int x, int y, ion::Vector2i& topLeft) const;
	int FindTerrainBeziers(int x, int y, int width, int height, std::vector<const ion::gamekit::BezierPath*>& beziers) const;
	void RemoveTerrainBezier(u32 index);
	int GetNumTerrainBeziers() const;

	// Generate NxN blocks and sort unique
	void GenerateBlocks(const Project& project, int blockWidth, int blockHeight);

	void Serialise(ion::io::Archive& archive);
	void Export(const Project& project, std::stringstream& stream) const;
	void Export(const Project& project, ion::io::File& file) const;
	void ExportBlocks(const Project& project, std::stringstream& stream, int blockWidth, int blockHeight) const;
	void ExportBlocks(const Project& project, ion::io::File& file, int blockWidth, int blockHeight) const;
	void ExportBlockMap(const Project& project, std::stringstream& stream, int blockWidth, int blockHeight) const;
	void ExportBlockMap(const Project& project, ion::io::File& file, int blockWidth, int blockHeight) const;

private:
	typedef u16 TTerrainTileDesc;

	struct Block
	{
		std::vector<TTerrainTileDesc> m_tiles;
		int uniqueIndex = -1;

		bool operator ==(const Block& rhs) const;
		void Export(const Project& project, std::stringstream& stream, int blockWidth, int blockHeight);
		void Export(const Project& project, ion::io::File& file, int blockWidth, int blockHeight);
	};

	int m_width;
	int m_height;

	//Tiles
	std::vector<TerrainTileId> m_collisionTiles;

	//Blocks
	std::vector<Block> m_blocks;
	std::vector<Block*> m_uniqueBlocks;

	//Terrain beziers
	std::vector<ion::gamekit::BezierPath> m_terrainBeziers;
};