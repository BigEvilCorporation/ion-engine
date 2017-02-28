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

#include "Actor.h"
#include "SpriteSheet.h"
#include "SpriteAnimation.h"

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
	Stamp();
	Stamp(StampId stampId, const Stamp& rhs);
	Stamp(StampId stampId, int width, int height);

	StampId GetId() const;
	int GetWidth() const;
	int GetHeight() const;

	void Resize(int w, int h, bool shiftRight, bool shiftDown);

	void SetTile(int x, int y, TileId tile);
	TileId GetTile(int x, int y) const;
	int GetTileIndex(TileId tileId) const;

	void SetTileFlags(int x, int y, u32 flags);
	u32 GetTileFlags(int x, int y) const;

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

		void Serialise(ion::io::Archive& archive)
		{
			archive.Serialise(m_id);
			archive.Serialise(m_flags);
		}

		TileId m_id;
		u32 m_flags;
	};

	std::vector<TileDesc> m_tiles;
	TStampAnimSheetMap m_stampAnimSheets;
};
