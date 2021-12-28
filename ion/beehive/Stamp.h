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
#include "Tileset.h"
#include "TerrainTile.h"

#include "Actor.h"
#include "SpriteSheet.h"
#include "SpriteAnimation.h"

#include <ion/gamekit/Bezier.h>

typedef u32 StampId;
static const StampId InvalidStampId = 0;

//Stamp animations are just sprite animations in disguise
typedef SpriteSheetId StampAnimSheetId;
typedef SpriteSheet StampAnimSheet;
typedef SpriteSheetFrame StampAnimSheetFrame;
typedef TSpriteSheetMap TStampAnimSheetMap;
static const StampAnimSheetId InvalidStampAnimSheetId = 0;

class Project;

class Stamp
{
public:
	typedef u16 TTerrainTileDesc;

	static const u16 s_collisionTileFlagMask = eCollisionTileFlagAll;
	static const u16 s_collisionTileTerrainIdMask = eCollisionTileFlagNone;

	Stamp();
	Stamp(StampId stampId, const Stamp& rhs);
	Stamp(StampId stampId, int width, int height);

	StampId GetId() const;
	int GetWidth() const;
	int GetHeight() const;
	int GetNumTerrainLayers() const;

	void Resize(int w, int h, bool shiftRight, bool shiftDown);
	void SetNumTerrainLayers(int numLayers);

	//Graphics tiles
	void SetTile(int x, int y, TileId tile);
	TileId GetTile(int x, int y) const;
	int GetTileIndex(TileId tileId) const;
	void SetTileFlags(int x, int y, u32 flags);
	u32 GetTileFlags(int x, int y) const;

	//Collision/terrain tiles
	void SetTerrainTile(int x, int y, TerrainTileId tile, int layer = 0);
	TerrainTileId GetTerrainTile(int x, int y, int layer = 0) const;
	void SetCollisionTileFlags(int x, int y, u16 flags, int layer = 0);
	u16 GetCollisionTileFlags(int x, int y, int layer = 0) const;

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

	void SetName(const std::string& name);
	const std::string& GetName() const;
	u32 GetNameHash() const;

	//Check all tiles in stamp are part of a sequential batch
	bool CheckTilesBatched() const;

	//Get all unique tiles in stamp, sorted sequentially
	int GetSortedUniqueTileBatch(std::vector<TileId>& tileBatch) const;

	//Find tile with lowest id
	TileId GetFirstTileId() const;

	//Animation sheets
	StampAnimSheetId CreateStampAnimSheet();
	void DeleteStampAnimSheet(StampAnimSheetId stampAnimSheetId);
	StampAnimSheet* GetStampAnimSheet(StampAnimSheetId stampAnimSheetId);
	const StampAnimSheet* GetStampAnimSheet(StampAnimSheetId stampAnimSheetId) const;
	StampAnimSheet* FindStampAnimSheet(const std::string& name);
	const TStampAnimSheetMap::const_iterator StampAnimSheetsBegin() const;
	const TStampAnimSheetMap::const_iterator StampAnimSheetsEnd() const;
	TStampAnimSheetMap::iterator StampAnimSheetsBegin();
	TStampAnimSheetMap::iterator StampAnimSheetsEnd();
	int GetStampAnimSheetCount() const;

	void Serialise(ion::io::Archive& archive);
	void Export(const Project& project, std::stringstream& stream) const;
	void Export(const Project& project, ion::io::File& file) const;
	void ExportStampAnims(const PlatformConfig& config, std::stringstream& stream) const;
	void ExportStampAnims(const PlatformConfig& config, ion::io::File& file) const;
	void ExportStampAnimSetup(const PlatformConfig& config, std::stringstream& stream) const;

	int GetBinarySize() const { return m_width * m_height * sizeof(u16); }

private:
	StampId m_id;
	int m_width;
	int m_height;
	std::string m_name;
	u32 m_nameHash;

	struct TileDesc
	{
		TileDesc() { m_id = 0; m_flags = 0; }
		TileDesc(TileId tileId) { m_id = tileId; m_flags = 0; }

		void Serialise(ion::io::Archive& archive)
		{
			archive.Serialise(m_id);
			archive.Serialise(m_flags);
		}

		TileId m_id;
		u32 m_flags;
	};

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

	std::vector<TileDesc> m_tiles;
	std::vector<std::vector<TerrainTileId>> m_terrainLayers;
	std::vector<TerrainBezier> m_terrainBeziers;
	TStampAnimSheetMap m_stampAnimSheets;
};
