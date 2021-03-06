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
#include <ion/io/Archive.h>
#include <ion/gamekit/Bezier.h>
#include <ion/maths/Fixed.h>

#include <set>
#include <vector>
#include <algorithm>
#include <cctype>

#include "Project.h"
#include "BMPReader.h"

#define HEX1(val) std::hex << std::setfill('0') << std::setw(1) << std::uppercase << (int)val << std::dec
#define HEX2(val) std::hex << std::setfill('0') << std::setw(2) << std::uppercase << (int)val << std::dec
#define HEX4(val) std::hex << std::setfill('0') << std::setw(4) << std::uppercase << (int)val << std::dec
#define HEX8(val) std::hex << std::setfill('0') << std::setw(8) << std::uppercase << (int)val << std::dec

Project::Project(PlatformConfig& defaultPatformConfig)
	: m_platformConfig(defaultPatformConfig)
	, m_tileset(m_platformConfig)
	, m_terrainTileset(m_platformConfig)
{
	m_paintColour = 0;
	m_paintTerrainTile = InvalidTerrainTileId;
	m_paintTile = InvalidTileId;
	m_eraseTile = InvalidTileId;
	m_backgroundTile = InvalidTileId;
	m_defaultTerrainTile = InvalidTerrainTileId;
	m_paintStamp = InvalidStampId;
	m_mapInvalidated = true;
	m_tilesInvalidated = true;
	m_terrainTilesInvalidated = true;
	m_stampsInvalidated = true;
	m_cameraInvalidated = true;
	m_blocksInvalidated = true;
	m_name = "untitled";
	m_gridSize = 1;
	m_showGrid = true;
	m_snapToGrid = false;
	m_showStampOutlines = true;
	m_showDisplayFrame = false;
	m_palettes.resize(s_maxPalettes);
	m_nextFreeStampId = 1;
	m_nextFreeGameObjectTypeId = 1;

	//Add default map
	m_editingMapId = CreateMap();
	m_editingCollisionMapId = CreateCollisionMap(m_editingMapId);
}

void Project::Clear()
{
	m_paintColour = 0;
	m_paintTerrainTile = InvalidTerrainTileId;
	m_paintTile = InvalidTileId;
	m_eraseTile = InvalidTileId;
	m_backgroundTile = InvalidTileId;
	m_defaultTerrainTile = InvalidTerrainTileId;
	m_paintStamp = InvalidStampId;
	m_mapInvalidated = true;
	m_tilesInvalidated = true;
	m_stampsInvalidated = true;
	m_blocksInvalidated = true;
	m_name = "untitled";
	m_palettes.resize(s_maxPalettes);

	for(int i = 0; i < s_maxPalettes; i++)
	{
		m_palettes[i].Clear();
	}

	m_maps[m_editingMapId].Clear();
	m_collisionMaps[m_editingCollisionMapId].Clear();
	m_tileset.Clear();
	m_stamps.clear();
	m_nextFreeStampId = 1;
	m_nextFreeGameObjectTypeId = 1;
}

bool Project::Load(const std::string& filename)
{
	ion::io::File file(filename, ion::io::File::eOpenRead);
	if(file.IsOpen())
	{
		ion::io::Archive archive(file, ion::io::Archive::Direction::In);
		Serialise(archive);

		InvalidateMap(true);
		InvalidateTiles(true);
		InvalidateStamps(true);

		m_filename = filename;

		std::map<u64, Tile> tiles;
		int count = m_tileset.GetCount();
		for(int i = 0; i < count; i++)
		{
			Tile tile = *m_tileset.GetTile(i);
			tile.CalculateHash();
			tiles.insert(std::make_pair(tile.GetHash(), tile));
		}

		//Load external resources
		if(!m_settings.gameObjectsExternalFile.empty())
		{
			//Import game objects file
			if(!ImportGameObjectTypes(m_settings.gameObjectsExternalFile))
			{
				ion::debug::Popup("Could not import external game object types, check settings.\nData may be lost if project is saved.", "Error");
			}
		}

		if(!m_settings.spriteActorsExternalFile.empty())
		{
			//Import sprites file
			if(!ImportActors(m_settings.spriteActorsExternalFile))
			{
				ion::debug::Popup("Could not import external sprite actors, check settings.\nData may be lost if project is saved.", "Error");
			}
		}

		return true;
	}

	return false;
}

bool Project::Save(const std::string& filename)
{
	ion::io::File file(filename, ion::io::File::eOpenWrite);
	if(file.IsOpen())
	{
		ion::io::Archive archive(file, ion::io::Archive::Direction::Out);
		Serialise(archive);
		m_filename = filename;

		//Save external resources
		if(!m_settings.gameObjectsExternalFile.empty())
		{
			//Export game objects file
			if(!ExportGameObjectTypes(m_settings.gameObjectsExternalFile, ExportFormat::Beehive, false))
			{
				ion::debug::Popup("Could not export external game object types, check settings.\nData may be lost if project is closed.", "Error");
			}
		}

		if(!m_settings.spriteActorsExternalFile.empty())
		{
			//Export sprites file
			if(!ExportActors(m_settings.spriteActorsExternalFile, ExportFormat::Beehive))
			{
				ion::debug::Popup("Could not export external sprite actors, check settings.\nData may be lost if project is closed.", "Error");
			}
		}

		return true;
	}

	return false;
}

void Project::Serialise(ion::io::Archive& archive)
{
	if(archive.GetDirection() == ion::io::Archive::Direction::In)
	{
		m_maps.clear();
		m_collisionMaps.clear();
		m_editingMapId = InvalidMapId;
		m_editingCollisionMapId = InvalidCollisionMapId;
	}

	archive.Serialise(m_settings, "settings");
	archive.Serialise(m_platformConfig, "platformConfig");
	archive.Serialise(m_name, "name");
	archive.Serialise(m_palettes, "palettes");
	archive.Serialise(m_paletteSlots, "paletteSlots");
	archive.Serialise(m_tileset, "tileset");
	archive.Serialise(m_maps, "maps");
	archive.Serialise(m_editingMapId, "editingMap");
	archive.Serialise(m_editingCollisionMapId, "editingCollisionMap");
	archive.Serialise(m_backgroundTile, "backgroundTile");
	archive.Serialise(m_terrainTileset, "terrainTileset");
	archive.Serialise(m_defaultTerrainTile, "defaultTerrainTile");
	archive.Serialise(m_collisionMaps, "collisionMaps");
	archive.Serialise(m_stamps, "stamps");
	archive.Serialise(m_animations, "animations");
	archive.Serialise(m_nextFreeStampId, "nextFreeStampId");
	archive.Serialise(m_nextFreeGameObjectTypeId, "nextFreeGameObjectTypeId");
	archive.Serialise(m_exportFilenames, "exportFilenames");

	if(m_settings.gameObjectsExternalFile.empty())
	{
		archive.Serialise(m_gameObjectTypes, "gameObjectTypes");
	}

	if(m_settings.spriteActorsExternalFile.empty())
	{ 
		archive.Serialise(m_actors, "actors");
	}

	if(archive.GetDirection() == ion::io::Archive::Direction::In && m_maps.size() == 0)
	{
		//Legacy, single map
		m_editingMapId = CreateMap();
		archive.Serialise(m_maps[m_editingMapId], "map");
	}

	if (archive.GetDirection() == ion::io::Archive::Direction::In && m_maps.size() > 0)
	{
		//Set project config
		for (TMapMap::iterator it = m_maps.begin(), end = m_maps.end(); it != end; ++it)
		{
			it->second.SetPlatformConfig(m_platformConfig);
		}
	}

	if(archive.GetDirection() == ion::io::Archive::Direction::In && m_collisionMaps.size() != m_maps.size())
	{
		//Legacy, single collision map
		if(m_collisionMaps.size() == 0)
		{
			m_editingCollisionMapId = CreateCollisionMap(m_editingMapId);
			archive.Serialise(m_collisionMaps[m_editingCollisionMapId], "collisionMap");

			std::vector<ion::gamekit::BezierPath> terrainBeziers;
			archive.Serialise(terrainBeziers, "terrainBeziers");

			for(int i = 0; i < terrainBeziers.size(); i++)
			{
				m_collisionMaps[m_editingCollisionMapId].AddTerrainBezier(terrainBeziers[i]);
			}
		}

		//Create remaining collision maps
		for(TMapMap::const_iterator it = m_maps.begin(), end = m_maps.end(); it != end; ++it)
		{
			if(it->first != m_editingCollisionMapId)
			{
				CollisionMapId collisionMapId = CreateCollisionMap(it->first, it->second.GetWidth(), it->second.GetHeight());
			}
		}
	}

	//Hack - fix for mismatched stamp sizes
	for(TMapMap::iterator it = m_maps.begin(), end = m_maps.end(); it != end; ++it)
	{
		for(TStampPosMap::iterator stampIt = it->second.StampsBegin(), stampEnd = it->second.StampsEnd(); stampIt != stampEnd; ++stampIt)
		{
			if(Stamp* stamp = GetStamp(stampIt->m_id))
			{
				stampIt->m_size = ion::Vector2i(stamp->GetWidth(), stamp->GetHeight());
			}
		}
	}
}

MapId Project::CreateMap()
{
	MapId mapId = ion::GenerateUUID64();
	m_maps.insert(std::make_pair(mapId, Map(m_platformConfig)));
	return mapId;
}

MapId Project::CreateMap(MapId mapId)
{
	m_maps.insert(std::make_pair(mapId, Map(m_platformConfig)));
	return mapId;
}

void Project::DeleteMap(MapId mapId)
{
	TMapMap::iterator it = m_maps.find(mapId);
	ion::debug::Assert(it != m_maps.end(), "Project::DeleteMap() - Invalid map ID");
	m_maps.erase(it);
}

Map& Project::GetMap(MapId mapId)
{
	TMapMap::iterator it = m_maps.find(mapId);
	ion::debug::Assert(it != m_maps.end(), "Project::GetMap() - Invalid map ID");
	return it->second;
}

const Map& Project::GetMap(MapId mapId) const
{
	TMapMap::const_iterator it = m_maps.find(mapId);
	ion::debug::Assert(it != m_maps.end(), "Project::GetMap() - Invalid map ID");
	return it->second;
}

Map* Project::FindMap(const std::string name)
{
	for(TMapMap::iterator it = m_maps.begin(), end = m_maps.end(); it != end; ++it)
	{
		if(ion::string::CompareNoCase(it->second.GetName(), name))
		{
			return &it->second;
		}
	}

	return NULL;
}

MapId Project::FindMapId(const std::string name)
{
	for(TMapMap::iterator it = m_maps.begin(), end = m_maps.end(); it != end; ++it)
	{
		if(ion::string::CompareNoCase(it->second.GetName(), name))
		{
			return it->first;
		}
	}

	return InvalidMapId;
}

Map& Project::GetEditingMap()
{
	return GetMap(m_editingMapId);
}

MapId Project::GetEditingMapId() const
{
	return m_editingMapId;
}

void Project::SetEditingMap(MapId mapId)
{
	m_editingMapId = mapId;
}

const TMapMap::const_iterator Project::MapsBegin() const
{
	return m_maps.begin();
}

const TMapMap::const_iterator Project::MapsEnd() const
{
	return m_maps.end();
}

TMapMap::iterator Project::MapsBegin()
{
	return m_maps.begin();
}

TMapMap::iterator Project::MapsEnd()
{
	return m_maps.end();
}

int Project::GetMapCount() const
{
	return m_maps.size();
}

CollisionMapId Project::CreateCollisionMap(CollisionMapId collisionMapId)
{
	m_collisionMaps.insert(std::make_pair(collisionMapId, CollisionMap(m_platformConfig)));
	return collisionMapId;
}

CollisionMapId Project::CreateCollisionMap(CollisionMapId collisionMapId, int width, int height)
{
	m_collisionMaps.insert(std::make_pair(collisionMapId, CollisionMap(m_platformConfig, width, height)));
	return collisionMapId;
}

void Project::DeleteCollisionMap(CollisionMapId collisionMapId)
{
	TCollisionMapMap::iterator it = m_collisionMaps.find(collisionMapId);
	ion::debug::Assert(it != m_collisionMaps.end(), "Project::DeleteCollisionMap() - Invalid collisionMap ID");
	m_collisionMaps.erase(it);
}

CollisionMap& Project::GetCollisionMap(CollisionMapId collisionMapId)
{
	TCollisionMapMap::iterator it = m_collisionMaps.find(collisionMapId);
	ion::debug::Assert(it != m_collisionMaps.end(), "Project::GetEditingCollisionMap() - Invalid collisionMap ID");
	return it->second;
}

const CollisionMap& Project::GetCollisionMap(CollisionMapId collisionMapId) const
{
	TCollisionMapMap::const_iterator it = m_collisionMaps.find(collisionMapId);
	ion::debug::Assert(it != m_collisionMaps.end(), "Project::GetEditingCollisionMap() - Invalid collisionMap ID");
	return it->second;
}

CollisionMap& Project::GetEditingCollisionMap()
{
	return GetCollisionMap(m_editingCollisionMapId);
}

CollisionMapId Project::GetEditingCollisionMapId() const
{
	return m_editingCollisionMapId;
}

void Project::SetEditingCollisionMap(CollisionMapId collisionMapId)
{
	m_editingCollisionMapId = collisionMapId;
}

const TCollisionMapMap::const_iterator Project::CollisionMapsBegin() const
{
	return m_collisionMaps.begin();
}

const TCollisionMapMap::const_iterator Project::CollisionMapsEnd() const
{
	return m_collisionMaps.end();
}

int Project::GetCollisionMapCount() const
{
	return m_collisionMaps.size();
}

int Project::AddPaletteSlot(Palette& palette)
{
	m_paletteSlots.push_back(palette);
	return m_paletteSlots.size() - 1;
}

Palette* Project::GetPaletteSlot(int slotIndex)
{
	ion::debug::Assert(slotIndex < GetNumPaletteSlots(), "Project::GetPaletteSlot() - Invalid slot index");
	return &m_paletteSlots[slotIndex];
}

const Palette* Project::GetPaletteSlot(int slotIndex) const
{
	ion::debug::Assert(slotIndex < GetNumPaletteSlots(), "Project::GetPaletteSlot() - Invalid slot index");
	return &m_paletteSlots[slotIndex];
}

int Project::GetNumPaletteSlots() const
{
	return m_paletteSlots.size();
}

void Project::SetActivePaletteSlot(PaletteId paletteId, int slotIndex)
{
	ion::debug::Assert(slotIndex < GetNumPaletteSlots(), "Project::GetPaletteSlot() - Invalid slot index");
	ion::debug::Assert(paletteId < GetNumPalettes(), "Project::GetPaletteSlot() - Invalid palettes index");

	Palette* source = GetPaletteSlot(slotIndex);
	Palette* dest = GetPalette(paletteId);
	*dest = *source;
}

void Project::ExportPaletteSlots(const std::string& filename, ExportFormat format)
{
	ion::io::File file(filename, ion::io::File::eOpenWrite);
	if(file.IsOpen())
	{
		ion::io::Archive archive(file, ion::io::Archive::Direction::Out);
		archive.Serialise(m_paletteSlots, "paletteSlots");
	}
}

void Project::ImportPaletteSlots(const std::string& filename)
{
	ion::io::File file(filename, ion::io::File::eOpenRead);
	if(file.IsOpen())
	{
		ion::io::Archive archive(file, ion::io::Archive::Direction::In);
		archive.Serialise(m_paletteSlots, "paletteSlots");
	}
}

void Project::SetBackgroundColour(u8 colourIdx)
{
	SwapPaletteEntries(colourIdx, 0);
}

void Project::SwapPaletteEntries(u8 colourA, u8 colourB)
{
	for(int i = 0; i < m_tileset.GetCount(); i++)
	{
		if(Tile* tile = m_tileset.GetTile(i))
		{
			for(int x = 0; x < m_platformConfig.tileWidth; x++)
			{
				for(int y = 0; y < m_platformConfig.tileHeight; y++)
				{
					u8 originalIdx = tile->GetPixelColour(x, y);
					if(originalIdx == colourB)
					{
						tile->SetPixelColour(x, y, colourA);
					}
					else if(originalIdx == colourA)
					{
						tile->SetPixelColour(x, y, colourB);
					}
				}
			}
		}
	}
}

void Project::CollapsePaletteSlots()
{
	m_paletteSlots.clear();

	for(int i = 0; i < GetNumPalettes(); i++)
	{
		if(m_palettes[i].GetUsedColourMask() > 0)
		{
			int slotIndex = AddPaletteSlot(m_palettes[i]);
		}
	}
}

void Project::DeleteTile(TileId tileId)
{
	TileId swapTileId = (TileId)m_tileset.GetCount() - 1;

	//Find all uses of tile on all maps, set blank
	for(TMapMap::iterator it = m_maps.begin(), end = m_maps.end(); it != end; ++it)
	{
		Map& map = it->second;
		int mapWidth = map.GetWidth();
		int mapHeight = map.GetHeight();

		for(int x = 0; x < mapWidth; x++)
		{
			for(int y = 0; y < mapHeight; y++)
			{
				TileId currTileId = map.GetTile(x, y);
				if(currTileId == tileId)
				{
					//Referencing deleted tile, set as invalid
					map.SetTile(x, y, InvalidTileId);
				}
				else if(currTileId == swapTileId)
				{
					//Referencing swap tile, set new id
					u32 flags = map.GetTileFlags(x, y);
					map.SetTile(x, y, tileId);
					map.SetTileFlags(x, y, flags);
				}
			}
		}
	}

	//Find all uses of tiles in stamps, set blank
	for(TStampMap::iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
	{
		for(int x = 0; x < it->second.GetWidth(); x++)
		{
			for(int y = 0; y < it->second.GetHeight(); y++)
			{
				TileId currTileId = it->second.GetTile(x, y);
				if(currTileId == tileId)
				{
					//Referencing deleted tile, set as invalid
					it->second.SetTile(x, y, InvalidTileId);
				}
				else if(currTileId == swapTileId)
				{
					//Referencing swap tile, set new id
					u32 flags = it->second.GetTileFlags(x, y);
					it->second.SetTile(x, y, tileId);
					it->second.SetTileFlags(x, y, flags);
				}
			}
		}
	}

	if(m_tileset.GetCount() > 0)
	{
		//Swap tiles
		Tile* tile1 = m_tileset.GetTile(tileId);
		Tile* tile2 = m_tileset.GetTile((TileId)m_tileset.GetCount() - 1);
		ion::debug::Assert(tile1 && tile2, "Project::DeleteTile() - Invalid tile");

		Tile tmpTile;
		tmpTile = *tile1;
		*tile1 = *tile2;
		*tile2 = tmpTile;

		//Erase last tile
		m_tileset.PopBackTile();
	}

	//Clear paint tile
	SetPaintTile(InvalidTileId);

	//Clear erase tile
	SetEraseTile(InvalidTileId);
}

void Project::SwapTiles(TileId tileId1, TileId tileId2)
{
	//Swap in tileset
	Tile* tile1 = m_tileset.GetTile(tileId1);
	Tile* tile2 = m_tileset.GetTile(tileId2);
	ion::debug::Assert(tile1 && tile2, "Project::SwapTiles() - Invalid tile");

	Tile tmpTile;
	tmpTile = *tile1;
	*tile1 = *tile2;
	*tile2 = tmpTile;

	//Find all uses of tiles on all maps, swap ids
	for(TMapMap::iterator it = m_maps.begin(), end = m_maps.end(); it != end; ++it)
	{
		Map& map = it->second;
		int mapWidth = map.GetWidth();
		int mapHeight = map.GetHeight();

		for(int x = 0; x < mapWidth; x++)
		{
			for(int y = 0; y < mapHeight; y++)
			{
				TileId currTileId = map.GetTile(x, y);
				if(currTileId == tileId1)
				{
					map.SetTile(x, y, tileId2);
				}
				else if(currTileId == tileId2)
				{
					map.SetTile(x, y, tileId1);
				}
			}
		}
	}

	//Find all uses of tiles in stamps, swap ids
	for(TStampMap::iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
	{
		for(int x = 0; x < it->second.GetWidth(); x++)
		{
			for(int y = 0; y < it->second.GetHeight(); y++)
			{
				TileId currTileId = it->second.GetTile(x, y);
				if(currTileId == tileId1)
				{
					it->second.SetTile(x, y, tileId2);
				}
				else if(currTileId == tileId2)
				{
					it->second.SetTile(x, y, tileId1);
				}
			}
		}
	}
}

void Project::SetBackgroundTile(TileId tileId)
{
	//Swap with tile 0
	if(tileId != 0)
	{
		Tile* tile0 = m_tileset.GetTile(0);
		Tile* bgTile = m_tileset.GetTile(tileId);

		if(tile0 && bgTile)
		{
			Tile temp = *tile0;
			*tile0 = *bgTile;
			*bgTile = temp;

			//Swap tile ids in all maps
			for(TMapMap::iterator it = m_maps.begin(), end = m_maps.end(); it != end; ++it)
			{
				for(int x = 0; x < it->second.GetWidth(); x++)
				{
					for(int y = 0; y < it->second.GetHeight(); y++)
					{
						TileId mapTile = it->second.GetTile(x, y);
						u32 mapFlags = it->second.GetTileFlags(x, y);

						if(mapTile == tileId)
						{
							it->second.SetTile(x, y, 0);
							it->second.SetTileFlags(x, y, mapFlags);
						}
						else if(mapTile == 0)
						{
							it->second.SetTile(x, y, tileId);
							it->second.SetTileFlags(x, y, mapFlags);
						}
					}
				}
			}

			//Swap tile ids in all stamps
			for(TStampMap::iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
			{
				for(int x = 0; x < it->second.GetWidth(); x++)
				{
					for(int y = 0; y < it->second.GetHeight(); y++)
					{
						TileId stampTile = it->second.GetTile(x, y);
						u32 stampFlags = it->second.GetTileFlags(x, y);

						if(stampTile == tileId)
						{
							it->second.SetTile(x, y, 0);
							it->second.SetTileFlags(x, y, stampFlags);
						}
						else if(stampTile == 0)
						{
							it->second.SetTile(x, y, tileId);
							it->second.SetTileFlags(x, y, stampFlags);
						}
					}
				}
			}
		}
	}

	//TODO: Redundant now
	m_backgroundTile = 0;
}

int Project::CleanupTiles()
{
	m_tileset.RebuildHashMap();

	std::set<TileId> usedTiles;

	//Collect all used tile ids from all maps
	for(TMapMap::iterator it = m_maps.begin(), end = m_maps.end(); it != end; ++it)
	{
		Map& map = it->second;
		int mapWidth = map.GetWidth();
		int mapHeight = map.GetHeight();

		for(int x = 0; x < mapWidth; x++)
		{
			for(int y = 0; y < mapHeight; y++)
			{
				TileId tileId = map.GetTile(x, y);

				//Ignore background tile
				if(tileId != m_backgroundTile)
				{
					usedTiles.insert(tileId);
				}
			}
		}
	}

	//Collect all used tile ids from stamps
	for(TStampMap::const_iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
	{
		const Stamp& stamp = it->second;

		for(int x = 0; x < stamp.GetWidth(); x++)
		{
			for(int y = 0; y < stamp.GetHeight(); y++)
			{
				TileId tileId = stamp.GetTile(x, y);
				usedTiles.insert(tileId);
			}
		}
	}

	std::set<TileId> unusedTiles;

	for(TileId i = 0; i < m_tileset.GetCount(); i++)
	{
		if(usedTiles.find(i) == usedTiles.end())
		{
			unusedTiles.insert(i);
		}
	}

	if(unusedTiles.size() > 0)
	{
		std::stringstream message;
		message << "Found " << unusedTiles.size() << " unused tiles, delete?";

		//TODO: wx message handler in ion::debug
		//if(wxMessageBox(message.str().c_str(), "Delete unused tiles", wxOK | wxCANCEL | wxICON_WARNING) == wxOK)
		{
			//Delete in reverse, popping from back
			for(std::set<TileId>::const_reverse_iterator it = unusedTiles.rbegin(), end = unusedTiles.rend(); it != end; ++it)
			{
				DeleteTile(*it);
			}
		}
	}

	//Calculate hashes for all tiles
	struct Duplicate
	{
		TileId original;
		TileId duplicate;
		Tileset::HashOrientation duplicateOrientation;
	};

	std::map<u64, TileId> tileMaps[Tileset::eNumHashOrientations];
	std::vector<Duplicate> duplicates;
	u64 hashes[Tileset::eNumHashOrientations];

	for(int i = 0; i < m_tileset.GetCount(); i++)
	{
		const Tile* tile = m_tileset.GetTile(i);
		m_tileset.CalculateHashes(*tile, hashes);
		bool duplicateFound = false;

		for(int j = 0; j < Tileset::eNumHashOrientations && !duplicateFound; j++)
		{
			std::map<u64, TileId>::iterator it = tileMaps[j].find(tile->GetHash());
			if(it != tileMaps[j].end())
			{
				Duplicate duplicate;
				duplicate.original = it->second;
				duplicate.duplicate = i;
				duplicate.duplicateOrientation = (Tileset::HashOrientation)j;
				duplicates.push_back(duplicate);
				duplicateFound = true;
			}
		}

		if(!duplicateFound)
		{
			for(int j = 0; j < Tileset::eNumHashOrientations; j++)
			{
				tileMaps[j].insert(std::make_pair(hashes[j], i));
			}
		}
	}

	if(duplicates.size() > 0)
	{
		std::stringstream message;
		message << "Found " << duplicates.size() << " duplicate tiles, delete?";

		//TODO: wx message handler in ion::debug
		//if(wxMessageBox(message.str().c_str(), "Delete duplicate tiles", wxOK | wxCANCEL | wxICON_WARNING) == wxOK)
		{
			for(int i = 0; i < duplicates.size(); i++)
			{
				//Find use of duplicate id in all maps
				for(TMapMap::iterator it = m_maps.begin(), end = m_maps.end(); it != end; ++it)
				{
					Map& map = it->second;
					int mapWidth = map.GetWidth();
					int mapHeight = map.GetHeight();

					for(int x = 0; x < mapWidth; x++)
					{
						for(int y = 0; y < mapHeight; y++)
						{
							if(map.GetTile(x, y) == duplicates[i].duplicate)
							{
								u32 originalFlags = map.GetTileFlags(x, y);

								//Replace duplicate with original
								map.SetTile(x, y, duplicates[i].original);

								//Orientate to match duplicate
								map.SetTileFlags(x, y, originalFlags ^ Tileset::s_orientationFlags[duplicates[i].duplicateOrientation]);
							}
						}
					}
				}

				//Find uses of duplicate id in stamps
				for(TStampMap::iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
				{
					for(int x = 0; x < it->second.GetWidth(); x++)
					{
						for(int y = 0; y < it->second.GetHeight(); y++)
						{
							if(it->second.GetTile(x, y) == duplicates[i].duplicate)
							{
								u32 originalFlags = it->second.GetTileFlags(x, y);

								//Replace duplicate with original
								it->second.SetTile(x, y, duplicates[i].original);

								//Orientate to match duplicate
								it->second.SetTileFlags(x, y, originalFlags ^ Tileset::s_orientationFlags[duplicates[i].duplicateOrientation]);
							}
						}
					}
				}
			}

			//Delete duplicates (in reverse, popping from back)
			for(int i = duplicates.size() - 1; i >= 0; --i)
			{
				DeleteTile(duplicates[i].duplicate);
			}
		}
	}

	m_tileset.RebuildHashMap();

	return unusedTiles.size() + duplicates.size();
}

ActorId Project::CreateActor()
{
	ActorId actorId = ion::GenerateUUID64();
	m_actors.insert(std::make_pair(actorId, Actor()));
	return actorId;
}

void Project::DeleteActor(ActorId actorId)
{
	TActorMap::iterator it = m_actors.find(actorId);
	if(it != m_actors.end())
	{
		m_actors.erase(it);
	}
}

Actor* Project::GetActor(ActorId actorId)
{
	Actor* actor = NULL;

	TActorMap::iterator it = m_actors.find(actorId);
	if(it != m_actors.end())
	{
		actor = &it->second;
	}

	return actor;
}

const Actor* Project::GetActor(ActorId actorId) const
{
	const Actor* actor = NULL;

	TActorMap::const_iterator it = m_actors.find(actorId);
	if(it != m_actors.end())
	{
		actor = &it->second;
	}

	return actor;
}

Actor* Project::FindActor(const std::string& name)
{
	for(TActorMap::iterator it = m_actors.begin(), end = m_actors.end(); it != end; ++it)
	{
		if(ion::string::CompareNoCase(it->second.GetName(), name))
		{
			return &it->second;
		}
	}

	return NULL;
}

const TActorMap::const_iterator Project::ActorsBegin() const
{
	return m_actors.begin();
}

const TActorMap::const_iterator Project::ActorsEnd() const
{
	return m_actors.end();
}

int Project::GetActorCount() const
{
	return m_actors.size();
}

bool Project::ExportActors(const std::string& filename, ExportFormat format)
{
	ion::io::File file(filename, ion::io::File::eOpenWrite);
	if(file.IsOpen())
	{
		ion::io::Archive archive(file, ion::io::Archive::Direction::Out);
		archive.Serialise(m_actors, "actors");
		return true;
	}

	return false;
}

bool Project::ImportActors(const std::string& filename)
{
	ion::io::File file(filename, ion::io::File::eOpenRead);
	if(file.IsOpen())
	{
		ion::io::Archive archive(file, ion::io::Archive::Direction::In);
		archive.Serialise(m_actors, "actors");
		return true;
	}

	return false;
}

AnimationId Project::CreateAnimation()
{
	AnimationId animationId = ion::GenerateUUID64();
	m_animations.insert(std::make_pair(animationId, Animation(animationId)));
	return animationId;
}

void Project::DeleteAnimation(AnimationId animationId)
{
	TAnimationMap::iterator it = m_animations.find(animationId);
	if(it != m_animations.end())
	{
		m_animations.erase(it);
	}
}

Animation* Project::GetAnimation(AnimationId animationId)
{
	Animation* animation = NULL;

	TAnimationMap::iterator it = m_animations.find(animationId);
	if(it != m_animations.end())
	{
		animation = &it->second;
	}

	return animation;
}

const Animation* Project::GetAnimation(AnimationId animationId) const
{
	const Animation* animation = NULL;

	TAnimationMap::const_iterator it = m_animations.find(animationId);
	if(it != m_animations.end())
	{
		animation = &it->second;
	}

	return animation;
}

const TAnimationMap::const_iterator Project::AnimationsBegin() const
{
	return m_animations.begin();
}

const TAnimationMap::const_iterator Project::AnimationsEnd() const
{
	return m_animations.end();
}

int Project::GetAnimationCount() const
{
	return m_animations.size();
}

StampId Project::AddStamp(int width, int height)
{
	StampId id = m_nextFreeStampId++;
	m_stamps.insert(std::make_pair(id, Stamp(id, width, height)));
	return id;
}

StampId Project::AddStamp(Stamp* stamp)
{
	StampId id = m_nextFreeStampId++;
	m_stamps.insert(std::make_pair(id, Stamp(id, *stamp)));
	return id;
}

void Project::DeleteStamp(StampId stampId)
{
	//Remove all uses of stamp on all maps
	std::vector<ion::Vector2i> stampUseCoords;

	for(TMapMap::iterator it = m_maps.begin(), end = m_maps.end(); it != end; ++it)
	{
		Map& map = it->second;
		int mapWidth = map.GetWidth();
		int mapHeight = map.GetHeight();

		for(TStampPosMap::const_iterator it = map.StampsBegin(), end = map.StampsEnd(); it != end; ++it)
		{
			if(it->m_id == stampId)
			{
				stampUseCoords.push_back(it->m_position);
			}
		}

		for(int i = 0; i < stampUseCoords.size(); i++)
		{
			map.RemoveStamp(stampId, stampUseCoords[i].x, stampUseCoords[i].y);
		}
	}

	//Delete stamp
	TStampMap::iterator it = m_stamps.find(stampId);
	if(it != m_stamps.end())
	{
		m_stamps.erase(it);
	}
}

Stamp* Project::GetStamp(StampId stampId)
{
	Stamp* stamp = NULL;

	TStampMap::iterator it = m_stamps.find(stampId);
	if(it != m_stamps.end())
	{
		stamp = &it->second;
	}

	return stamp;
}

const Stamp* Project::GetStamp(StampId stampId) const
{
	const Stamp* stamp = NULL;

	TStampMap::const_iterator it = m_stamps.find(stampId);
	if(it != m_stamps.end())
	{
		stamp = &it->second;
	}

	return stamp;
}

Stamp* Project::FindStamp(const std::string& name)
{
	for(TStampMap::iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
	{
		if(it->second.GetName() == name)
		{
			return &it->second;
		}
	}

	return NULL;
}

StampId Project::FindDuplicateStamp(Stamp* stamp) const
{
	StampId foundStampId = InvalidStampId;

	for(TStampMap::const_iterator it = m_stamps.begin(), end = m_stamps.end(); it != end && foundStampId == InvalidStampId; ++it)
	{
		if(it->second.GetWidth() == stamp->GetWidth() && it->second.GetHeight() == stamp->GetHeight())
		{
			bool tilesMatch = true;

			for(int x = 0; x < it->second.GetWidth() && tilesMatch; x++)
			{
				for(int y = 0; y < it->second.GetHeight() && tilesMatch; y++)
				{
					const Tile* tileA = GetTileset().GetTile(stamp->GetTile(x, y));
					const Tile* tileB = GetTileset().GetTile(it->second.GetTile(x, y));
					const u64 hashA = tileA ? tileA->GetHash() : 0;
					const u64 hashB = tileB ? tileB->GetHash() : 0;
					const u32 flagsA = stamp->GetTileFlags(x, y);
					const u32 flagsB = it->second.GetTileFlags(x, y);

					if(((tileA != NULL) != (tileB != NULL)) || (hashA != hashB) || (flagsA != flagsB))
					{
						tilesMatch = false;
					}
				}
			}

			if(tilesMatch)
				foundStampId = it->first;
		}
	}

	return foundStampId;
}

const TStampMap::const_iterator Project::StampsBegin() const
{
	return m_stamps.begin();
}

const TStampMap::const_iterator Project::StampsEnd() const
{
	return m_stamps.end();
}

int Project::GetStampCount() const
{
	return m_stamps.size();
}

void Project::SubstituteStamp(StampId stampToReplace, StampId substitution)
{
	for(TMapMap::iterator mapIt = m_maps.begin(), mapEnd = m_maps.end(); mapIt != mapEnd; ++mapIt)
	{
		for(TStampPosMap::iterator stampIt = mapIt->second.GetStamps().begin(), stampEnd = mapIt->second.GetStamps().end(); stampIt != stampEnd; ++stampIt)
		{
			if(stampIt->m_id == stampToReplace)
			{
				stampIt->m_id = substitution;
			}
		}
	}
}

void Project::SortStampTilesSequentially(Stamp* stamp)
{
	//Get all unique tiles
	std::vector<TileId> tiles;
	tiles.reserve(stamp->GetWidth() * stamp->GetHeight());

	for(int y = 0; y < stamp->GetHeight(); y++)
	{
		for(int x = 0; x < stamp->GetWidth(); x++)
		{
			TileId tileId = stamp->GetTile(x, y);

			if(std::find(tiles.begin(), tiles.end(), tileId) == tiles.end())
			{
				tiles.push_back(tileId);
			}
		}
	}

	//Sort
	std::sort(tiles.begin(), tiles.end(), [](const TileId& a, const TileId& b) { return a < b; });

	//Make tiles sequential
	int index = tiles.front();
	for(int i = 0; i < tiles.size(); i++, index++)
	{
		if(index != tiles[i])
		{
			SwapTiles(index, tiles[i]);

#if defined _DEBUG
			tiles[i] = index;
#endif
		}
	}

#if defined _DEBUG
	ion::debug::Assert(stamp->CheckTilesBatched(), "Project::SortStampTilesSequentially() - Error, tiles still not in sequential batch after sorting");
#endif
}

int Project::CleanupStamps()
{
	std::set<StampId> unusedStamps;

	for(TStampMap::const_iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
	{
		if(it->second.GetStampAnimSheetCount() == 0)
		{
			unusedStamps.insert(it->first);
		}
	}

	for(TMapMap::const_iterator mapIt = m_maps.begin(), mapEnd = m_maps.end(); mapIt != mapEnd; ++mapIt)
	{
		for(TStampPosMap::const_iterator stampIt = mapIt->second.GetStamps().begin(), stampEnd = mapIt->second.GetStamps().end(); stampIt != stampEnd; ++stampIt)
		{
			unusedStamps.erase(stampIt->m_id);
		}
	}
	
	if(unusedStamps.size() > 0)
	{
		std::stringstream message;
		message << "Found " << unusedStamps.size() << " unused stamps, delete?";

		//TODO: wx message handler in ion::debug
		//if(wxMessageBox(message.str().c_str(), "Delete unused stamps", wxOK | wxCANCEL | wxICON_WARNING) == wxOK)
		{
			for(std::set<StampId>::const_iterator it = unusedStamps.begin(), end = unusedStamps.end(); it != end; ++it)
			{
				DeleteStamp(*it);
			}
		}
	}

	return unusedStamps.size();
}

void Project::DeleteTerrainTile(TerrainTileId terrainTileId)
{
	TerrainTileId swapTerrainTileId = (TerrainTileId)m_terrainTileset.GetCount() - 1;

	//Find all uses of terrain tile on all maps, set blank
	for(TCollisionMapMap::iterator it = m_collisionMaps.begin(), end = m_collisionMaps.end(); it != end; ++it)
	{
		CollisionMap& collisionMap = it->second;
		int mapWidth = collisionMap.GetWidth();
		int mapHeight = collisionMap.GetHeight();

		for(int x = 0; x < collisionMap.GetWidth(); x++)
		{
			for(int y = 0; y < collisionMap.GetHeight(); y++)
			{
				TerrainTileId currTerrainTileId = collisionMap.GetTerrainTile(x, y);
				if(currTerrainTileId == terrainTileId)
				{
					//Referencing deleted tile, set as invalid
					collisionMap.SetTerrainTile(x, y, InvalidTerrainTileId);
				}
				else if(currTerrainTileId == swapTerrainTileId)
				{
					//Referencing swap tile, set new id
					collisionMap.SetTerrainTile(x, y, terrainTileId);
				}
			}
		}
	}

	if(m_terrainTileset.GetCount() > 0)
	{
		if(swapTerrainTileId != terrainTileId)
		{
			//Swap terrain tile
			TerrainTile* terrainTile1 = m_terrainTileset.GetTerrainTile(terrainTileId);
			TerrainTile* terrainTile2 = m_terrainTileset.GetTerrainTile(swapTerrainTileId);
			ion::debug::Assert(terrainTile1 && terrainTile2, "Project::DeleteTerrainTile() - Invalid terrain tile");

			TerrainTile tmpTerrainTile;
			tmpTerrainTile = *terrainTile1;
			*terrainTile1 = *terrainTile2;
			*terrainTile2 = tmpTerrainTile;
		}

		//Erase last terrain tile
		m_terrainTileset.PopBackTerrainTile();

		if(swapTerrainTileId != terrainTileId)
		{
			//Id belonging to swapped hash has changed, reinsert into hash map
			m_terrainTileset.HashChanged(terrainTileId);
		}
	}

	//Clear paint terrain tile
	SetPaintTerrainTile(InvalidTerrainTileId);
}

void Project::SwapTerrainTiles(TerrainTileId tileId1, TerrainTileId tileId2)
{

}

void Project::SetDefaultTerrainTile(TerrainTileId tileId)
{
	m_defaultTerrainTile = tileId;
}

int Project::CleanupTerrainTiles(bool prompt)
{
	//Backup default collision tile
	TerrainTile* defaultTile = (m_defaultTerrainTile != InvalidTerrainTileId) ? GetTerrainTileset().GetTerrainTile(m_defaultTerrainTile) : NULL;
	u64 defaultTileHash = defaultTile ? defaultTile->CalculateHash() : 0;

	//Collect all used TerrainTile ids from all maps
	std::vector<bool> usedTerrainTiles;
	usedTerrainTiles.resize(m_terrainTileset.GetCount(), false);
	int usedTileCount = 0;

	//Always add default tile
	if(m_defaultTerrainTile != InvalidTerrainTileId)
	{
		usedTerrainTiles[m_defaultTerrainTile] = true;
	}

	for(TCollisionMapMap::iterator it = m_collisionMaps.begin(), end = m_collisionMaps.end(); it != end; ++it)
	{
		CollisionMap& collisionMap = it->second;
		int mapWidth = collisionMap.GetWidth();
		int mapHeight = collisionMap.GetHeight();

		for(int x = 0; x < mapWidth; x++)
		{
			for(int y = 0; y < mapHeight; y++)
			{
				TerrainTileId terrainTileId = collisionMap.GetTerrainTile(x, y);

				if(terrainTileId != InvalidTerrainTileId)
				{
					if(!usedTerrainTiles[terrainTileId])
					{
						usedTerrainTiles[terrainTileId] = true;
						usedTileCount++;
					}
				}
			}
		}

		//Cleanup zero-sized beziers
		for(int i = 0; i < collisionMap.GetNumTerrainBeziers();)
		{
			ion::gamekit::BezierPath* bezier = collisionMap.GetTerrainBezier(i);
			if(bezier->GetNumCurves() == 0)
			{
				collisionMap.RemoveTerrainBezier(i);
			}
			else
			{
				i++;
			}
		}
	}

	std::vector<TerrainTileId> unusedTerrainTiles;
	unusedTerrainTiles.reserve(m_terrainTileset.GetCount() - usedTileCount);

	for(TerrainTileId i = 0; i < m_terrainTileset.GetCount(); i++)
	{
		if(!usedTerrainTiles[i])
		{
			unusedTerrainTiles.push_back(i);
		}
	}

	if(unusedTerrainTiles.size() > 0)
	{
		std::stringstream message;
		message << "Found " << unusedTerrainTiles.size() << " unused collision tiles, delete?";

		//Sort
		std::sort(unusedTerrainTiles.begin(), unusedTerrainTiles.end());

		//TODO: wx message handler in ion::debug
		//if(!prompt || wxMessageBox(message.str().c_str(), "Delete unused collision tiles", wxOK | wxCANCEL | wxICON_WARNING) == wxOK)
		{
			//Delete in reverse, popping from back
			for(std::vector<TerrainTileId>::const_reverse_iterator it = unusedTerrainTiles.rbegin(), end = unusedTerrainTiles.rend(); it != end; ++it)
			{
				DeleteTerrainTile(*it);
			}
		}
	}

	//Rebuild hash map
	GetTerrainTileset().RebuildHashMap();

	//Default might have moved
	m_defaultTerrainTile = GetTerrainTileset().FindDuplicate(defaultTileHash);

	//Calculate hashes for all TerrainTiles
	struct Duplicate
	{
		TerrainTileId original;
		TerrainTileId duplicate;
	};

	std::map<u64, TerrainTileId> terrainTileMap;
	std::vector<Duplicate> duplicates;

	for(int i = 0; i < m_terrainTileset.GetCount(); i++)
	{
		TerrainTile* terrainTile = m_terrainTileset.GetTerrainTile(i);

		terrainTile->CalculateHash();
		u64 hash = terrainTile->GetHash();
		bool duplicateFound = false;

		if(i != m_defaultTerrainTile)
		{
			std::map<u64, TerrainTileId>::iterator it = terrainTileMap.find(hash);
			if(it != terrainTileMap.end())
			{
				Duplicate duplicate;
				duplicate.original = it->second;
				duplicate.duplicate = i;
				duplicates.push_back(duplicate);
				duplicateFound = true;
			}

			if(!duplicateFound)
			{
				terrainTileMap.insert(std::make_pair(hash, i));
			}
		}
	}

	if(duplicates.size() > 0)
	{
		std::stringstream message;
		message << "Found " << duplicates.size() << " duplicate collision tiles, delete?";

		//TODO: wx message handler in ion::debug
		//if(!prompt || wxMessageBox(message.str().c_str(), "Delete duplicate collision tiles", wxOK | wxCANCEL | wxICON_WARNING) == wxOK)
		{
			for(int i = 0; i < duplicates.size(); i++)
			{
				//Find use of duplicate id in all maps
				for(TCollisionMapMap::iterator it = m_collisionMaps.begin(), end = m_collisionMaps.end(); it != end; ++it)
				{
					CollisionMap& collisionMap = it->second;

					for(int x = 0; x < collisionMap.GetWidth(); x++)
					{
						for(int y = 0; y < collisionMap.GetHeight(); y++)
						{
							if(collisionMap.GetTerrainTile(x, y) == duplicates[i].duplicate)
							{
								//Replace duplicate with original
								collisionMap.SetTerrainTile(x, y, duplicates[i].original);
							}
						}
					}
				}
			}

			//Delete duplicates (in reverse, popping from back)
			for(int i = duplicates.size() - 1; i >= 0; --i)
			{
				DeleteTerrainTile(duplicates[i].duplicate);
			}
		}
	}

	//Rebuild hash map
	GetTerrainTileset().RebuildHashMap();

	//Default might have moved
	m_defaultTerrainTile = GetTerrainTileset().FindDuplicate(defaultTileHash);

	return unusedTerrainTiles.size() + duplicates.size();
}

bool Project::GenerateTerrainFromBeziers(int granularity)
{
	//Clear all terrain tiles
	m_terrainTileset.Clear();

	//Create blank tile, use as default
	TerrainTileId blankTileId = m_terrainTileset.AddTerrainTile();
	SetDefaultTerrainTile(blankTileId);

	//Clear all terrain entries from all maps
	for(TCollisionMapMap::iterator it = m_collisionMaps.begin(), end = m_collisionMaps.end(); it != end; ++it)
	{
		CollisionMap& collisionMap = it->second;
		int mapWidth = collisionMap.GetWidth();
		int mapHeight = collisionMap.GetHeight();

		for(int x = 0; x < mapWidth; x++)
		{
			for(int y = 0; y < mapHeight; y++)
			{
				collisionMap.SetTerrainTile(x, y, InvalidTerrainTileId);
				collisionMap.SetCollisionTileFlags(x, y, collisionMap.GetCollisionTileFlags(x, y) & (eCollisionTileFlagSolid | eCollisionTileFlagHole));
			}
		}
	}

	for(TCollisionMapMap::iterator it = m_collisionMaps.begin(), end = m_collisionMaps.end(); it != end; ++it)
	{
		CollisionMap& collisionMap = it->second;
		int mapWidth = collisionMap.GetWidth();
		int mapHeight = collisionMap.GetHeight();

		const int tileWidth = GetPlatformConfig().tileWidth;
		const int tileHeight = GetPlatformConfig().tileHeight;

		const int mapWidthPixels = (collisionMap.GetWidth() * tileWidth);
		const int mapHeightPixels = (collisionMap.GetHeight() * tileHeight);

		ion::Vector2i prevTilePos;
		TerrainTileId tileId = InvalidTileId;

		//Reserve tiles
		std::vector<std::pair<TerrainTile, u16>> terrainTiles;
		int numTiles = mapWidth * mapHeight;

		terrainTiles.clear();
		terrainTiles.reserve(numTiles);
		for(int i = 0; i < numTiles; i++)
		{
			terrainTiles.push_back(std::make_pair(TerrainTile(tileWidth, tileHeight), 0));
		}

		//Follow paths, generate terrain height tiles
		for(int i = 0; i < collisionMap.GetNumTerrainBeziers(); i++)
		{
			ion::gamekit::BezierPath* bezier = collisionMap.GetTerrainBezier(i);
			u16 terrainFlags = collisionMap.GetTerrainBezierFlags(i);
			const int maxPoints = bezier->GetNumPoints();

			if(bezier->GetNumCurves() > 0)
			{
				//Get all spline points
				std::vector<ion::Vector2> points;
				points.reserve(maxPoints);
				bezier->GetPositions(points, 0.0f, 1.0f, granularity);

				//Draw all tiles in spline
				for(int posIdx = 0; posIdx < points.size(); posIdx++)
				{
					//Get position
					const ion::Vector2 pixelPos(ion::maths::Floor(points[posIdx].x), (float)mapHeightPixels - ion::maths::Round(points[posIdx].y));
					const ion::Vector2i tilePos(ion::maths::Floor(pixelPos.x / (float)tileWidth), ion::maths::Floor(pixelPos.y / (float)tileHeight));

					if(tilePos.x >= 0 && tilePos.x < mapWidth && tilePos.y >= 0 && tilePos.y < mapHeight)
					{
						//Get tile
						TerrainTile& currentTile = terrainTiles[(tilePos.y * mapWidth) + tilePos.x].first;
						u16& currentFlags = terrainTiles[(tilePos.y * mapWidth) + tilePos.x].second;

						//Draw height at X
						int pixelX = pixelPos.x - (tilePos.x * tileWidth);
						int pixelY = pixelPos.y - (tilePos.y * tileHeight);
						const int height = ion::maths::Clamp(tileHeight - pixelY, 1, tileHeight);
						currentTile.SetHeight(pixelX, height);

						//Set flags
						if(tilePos.x != prevTilePos.x || tilePos.y != prevTilePos.y)
						{
							currentFlags |= terrainFlags;
							prevTilePos = tilePos;
						}
					}
				}
			}
		}

		const bool approximateCurves = true;

		if(approximateCurves)
		{
			//Reduce unique tiles by approximating curves using start/end pos only
			for(int i = 0; i < terrainTiles.size(); i++)
			{
				TerrainTile& terrainTile = terrainTiles[i].first;

				//Get min and max X bounds, and determine if tile contains gaps
				bool containsGaps = false;
				int x1 = -1;
				int x2 = -1;

				for(int x = 0; x < tileWidth && !containsGaps; x++)
				{
					u8 y = terrainTile.GetHeight(x);
					
					if(y == 0)
					{
						//If left marker found, and right marker not found, use as right marker
						if(x1 != -1 && x2 == -1)
						{
							x2 = x - 1;
						}
					}
					else
					{
						//If left marker not found, use as left marker
						if(x1 == -1)
						{
							x1 = x;
						}

						//If both markers found, this is a gap
						if(x1 != -1 && x2 != -1)
						{
							containsGaps = true;
						}
					}
				}

				if(x1 == -1)
					x1 = 0;
				if(x2 == -1)
					x2 = tileWidth - 1;

				if(!containsGaps && x1 != x2)
				{
					int y1 = terrainTile.GetHeight(x1);
					int y2 = terrainTile.GetHeight(x2);

					// Bresenham's line algorithm
					const bool steep = (ion::maths::Abs(y2 - y1) > ion::maths::Abs(x2 - x1));
					if(steep)
					{
						std::swap(x1, y1);
						std::swap(x2, y2);
					}

					if(x1 > x2)
					{
						std::swap(x1, x2);
						std::swap(y1, y2);
					}

					const float dx = x2 - x1;
					const float dy = ion::maths::Abs(y2 - y1);

					float error = dx / 2.0f;
					const int ystep = (y1 < y2) ? 1 : -1;
					int y = (int)y1;

					const int maxX = (int)x2;

					for(int x = (int)x1; x < maxX; x++)
					{
						if(steep)
						{
							terrainTile.SetHeight(y, x);
						}
						else
						{
							terrainTile.SetHeight(x, y);
						}

						error -= dy;
						if(error < 0)
						{
							y += ystep;
							error += dx;
						}
					}
				}
			}
		}

		//Find duplicates/draw to map
		for(int x = 0; x < mapWidth; x++)
		{
			for(int y = 0; y < mapHeight; y++)
			{
				if(x >= 0 && x < mapWidth && y >= 0 && y < mapHeight)
				{
					//Get generated tile
					TerrainTile& currentTile = terrainTiles[(y * mapWidth) + x].first;
					u16 currentFlags = terrainTiles[(y * mapWidth) + x].second;

					//If empty tile, keep existing map entry
					bool empty = true;
					for(int i = 0; i < tileWidth && empty; i++)
					{
						empty = currentTile.GetHeight(i) == 0;
					}

					if(!empty)
					{
						//Calculate hash
						currentTile.CalculateHash();

						//Find duplicate tile
						tileId = m_terrainTileset.FindDuplicate(currentTile.GetHash());

						if(tileId == InvalidTerrainTileId)
						{
							//Add as new collision tile
							tileId = m_terrainTileset.AddTerrainTile(currentTile);

							if(tileId == InvalidTerrainTileId)
							{
								//Out of tiles
								return false;
							}
						}
						else
						{
							TerrainTile* duplicate = m_terrainTileset.GetTerrainTile(tileId);
							if(!(*duplicate == currentTile))
							{
								ion::debug::error << "Terrain tile hash collision" << ion::debug::end;
							}
						}

						//Set on map
						collisionMap.SetTerrainTile(x, y, tileId);

						//Clear water/special flag
						u16 originalFlags = collisionMap.GetCollisionTileFlags(x, y);
						originalFlags &= ~(eCollisionTileFlagWater | eCollisionTileFlagSpecial);

						collisionMap.SetCollisionTileFlags(x, y, originalFlags | currentFlags);
					}
				}
			}
		}
	}

	InvalidateTerrainTiles(false);

	return true;
}

void Project::GenerateTerrain(const std::vector<ion::Vector2i>& graphicTiles)
{
	CollisionMap& collisionMap = m_collisionMaps[m_editingCollisionMapId];
	int mapWidth = collisionMap.GetWidth();
	int mapHeight = collisionMap.GetHeight();

	const int tileWidth = GetPlatformConfig().tileWidth;
	const int tileHeight = GetPlatformConfig().tileHeight;

	std::map<TileId, TerrainTileId> generatedTiles;

	for(int i = 0; i < graphicTiles.size(); i++)
	{
		const ion::Vector2i& position = graphicTiles[i];

		//Ignore the topmost tiles
		if(position.y > 0)
		{
			TileId graphicTileId = GetTileAtPosition(position);

			if(const Tile* graphicTile = m_tileset.GetTile(graphicTileId))
			{
				//Check if already generated
				std::map<TileId, TerrainTileId>::iterator it = generatedTiles.find(graphicTileId);
				if(it != generatedTiles.end())
				{
					collisionMap.SetTerrainTile(position.x, position.y, it->second);
				}
				else
				{
					//Generate height map from tile
					std::vector<u8> heightMap;
					GenerateHeightMap(*graphicTile, heightMap);

					//If height map isn't empty (whole row is 0)
					if(std::count(heightMap.begin(), heightMap.end(), 0) != tileWidth)
					{
						bool spaceForTerrain = true;

						//If height map fills top row (whole row of 1)
						if(std::count(heightMap.begin(), heightMap.end(), 1) == tileWidth)
						{
							//Generate ceiling map from tile above
							TileId upperTile = GetTileAtPosition(ion::Vector2i(position.x, position.y - 1));
							if(const Tile* graphicTile = m_tileset.GetTile(upperTile))
							{
								std::vector<u8> ceilingMap;
								GenerateCeilingMap(*graphicTile, ceilingMap);

								//If there's no space (whole row of tileHeight)
								if(std::count(ceilingMap.begin(), ceilingMap.end(), tileHeight) == tileWidth)
								{
									//Don't bother
									spaceForTerrain = false;
								}
							}
						}

						if(spaceForTerrain)
						{
							//Add new collision tile
							TerrainTileId terrainTileId = m_terrainTileset.AddTerrainTile();
							TerrainTile* terrainTile = m_terrainTileset.GetTerrainTile(terrainTileId);

							//Create collision tile from height map
							for(int x = 0; x < tileWidth; x++)
							{
								if(heightMap[x] > 0)
								{
									terrainTile->SetHeight(x, heightMap[x] - 1);
								}
							}

							//TODO: Find duplicate

							//Set on map
							collisionMap.SetTerrainTile(position.x, position.y, terrainTileId);

							//Add to cache
							generatedTiles.insert(std::make_pair(graphicTileId, terrainTileId));
						}
					}
				}
			}
		}
	}
}

void Project::GenerateHeightMap(const Tile& graphicTile, std::vector<u8>& heightMap) const
{
	const int tileWidth = GetPlatformConfig().tileWidth;
	const int tileHeight = GetPlatformConfig().tileHeight;

	heightMap.resize(tileWidth);
	std::fill(heightMap.begin(), heightMap.end(), 0);

	for(int x = 0; x < tileWidth; x++)
	{
		bool foundHeight = false;

		for(int y = 0; y < tileHeight && !foundHeight; y++)
		{
			//If pixel not background colour
			if(graphicTile.GetPixelColour(x, y) != 0)
			{
				//Use height
				heightMap[x] = y + 1;
				foundHeight = true;
			}
		}
	}
}

void Project::GenerateCeilingMap(const Tile& graphicTile, std::vector<u8>& ceilingMap) const
{
	const int tileWidth = GetPlatformConfig().tileWidth;
	const int tileHeight = GetPlatformConfig().tileHeight;

	ceilingMap.resize(tileWidth);
	std::fill(ceilingMap.begin(), ceilingMap.end(), 0);

	for(int x = 0; x < tileWidth; x++)
	{
		bool foundHeight = false;

		
		for(int y = tileHeight - 1; y >= 0 && !foundHeight; y--)
		{
			//If pixel not background colour
			if(graphicTile.GetPixelColour(x, y) != 0)
			{
				//Use height
				ceilingMap[x] = y + 1;
				foundHeight = true;
			}
		}
	}
}

TileId Project::GetTileAtPosition(const ion::Vector2i& position)
{
	TileId tileId = InvalidTileId;

	Map& map = m_maps[m_editingMapId];

	//Find stamp under cursor first
	ion::Vector2i stampPos;
	u32 stampFlags = 0;
	u32 mapEntryIndex = 0;
	StampId stampId = map.FindStamp(position.x, position.y, stampPos, stampFlags, mapEntryIndex);
	if(stampId)
	{
		//Get from stamp
		if(Stamp* stamp = GetStamp(stampId))
		{
			ion::Vector2i offset = position - stampPos;
			tileId = stamp->GetTile(offset.x, offset.y);
		}
	}
	else
	{
		//Pick tile
		tileId = map.GetTile(position.x, position.y);
	}

	return tileId;
}

GameObjectTypeId Project::AddGameObjectType()
{
	GameObjectTypeId typeId = m_nextFreeGameObjectTypeId++;
	m_gameObjectTypes.insert(std::make_pair(typeId, GameObjectType(typeId)));
	return typeId;
}

void Project::RemoveGameObjectType(GameObjectTypeId typeId)
{
	m_gameObjectTypes.erase(typeId);
}

GameObjectType* Project::GetGameObjectType(GameObjectTypeId typeId)
{
	GameObjectType* gameObjectType = NULL;

	if(typeId != InvalidGameObjectTypeId)
	{
		TGameObjectTypeMap::iterator it = m_gameObjectTypes.find(typeId);
		if(it != m_gameObjectTypes.end())
			gameObjectType = &it->second;
	}
	
	return gameObjectType;
}

const GameObjectType* Project::GetGameObjectType(GameObjectTypeId typeId) const
{
	const GameObjectType* gameObjectType = NULL;

	if(typeId != InvalidGameObjectTypeId)
	{
		TGameObjectTypeMap::const_iterator it = m_gameObjectTypes.find(typeId);
		if(it != m_gameObjectTypes.end())
			gameObjectType = &it->second;
	}

	return gameObjectType;
}

const TGameObjectTypeMap& Project::GetGameObjectTypes() const
{
	return m_gameObjectTypes;
}

bool Project::ExportGameObjectTypes(const std::string& filename, ExportFormat format, bool minimal)
{
	ion::io::File file(filename, ion::io::File::eOpenWrite);
	if(file.IsOpen())
	{
		ion::io::Archive archive(file, ion::io::Archive::Direction::Out);

		if (minimal)
		{
			archive.SetContentType(ion::io::Archive::Content::Minimal);
		}

		archive.Serialise(m_gameObjectTypes, "gameObjectTypes");
		archive.Serialise(m_nextFreeGameObjectTypeId, "nextFreeGameObjTypeId");
		return true;
	}

	return false;
}

bool Project::ImportGameObjectTypes(const std::string& filename)
{
	ion::io::File file(filename, ion::io::File::eOpenRead);
	if(file.IsOpen())
	{
		ion::io::Archive archive(file, ion::io::Archive::Direction::In);
		archive.Serialise(m_gameObjectTypes, "gameObjectTypes");
		archive.Serialise(m_nextFreeGameObjectTypeId, "nextFreeGameObjTypeId");
		return true;
	}

	return false;
}

void Project::SetPaintColour(u8 colourIdx)
{
	m_paintColour = colourIdx;
}

u8 Project::GetPaintColour() const
{
	return m_paintColour;
}

void Project::SetPaintTerrainTile(TerrainTileId tile)
{
	m_paintTerrainTile = tile;
}

TerrainTileId Project::GetPaintTerrainTile() const
{
	return m_paintTerrainTile;
}

void Project::SetPaintTile(TileId tile)
{
	m_paintTile = tile;
}

TileId Project::GetPaintTile() const
{
	return m_paintTile;
}

void Project::SetEraseTile(TileId tile)
{
	m_eraseTile = tile;
}

TileId Project::GetEraseTile() const
{
	return m_eraseTile;
}

void Project::SetPaintStamp(StampId stamp)
{
	m_paintStamp = stamp;
}

StampId Project::GetPaintStamp() const
{
	return m_paintStamp;
}

void Project::SetPaintGameObjectType(GameObjectTypeId typeId)
{
	m_paintGameObjectType = typeId;
}

GameObjectTypeId Project::GetPaintGameObjectType() const
{
	return m_paintGameObjectType;
}

bool Project::FindPalette(Colour* pixels, u32 useablePalettes, PaletteId& paletteId, PaletteId& closestPalette, int& closestColourCount) const
{
	//Set of found colour idxs
	std::set<int> colourMatches;

	const int tileWidth = GetPlatformConfig().tileWidth;
	const int tileHeight = GetPlatformConfig().tileHeight;

	//For each palette
	for(int paletteIdx = 0; paletteIdx < s_maxPalettes; paletteIdx++)
	{
		if(useablePalettes & (1 << paletteIdx))
		{
			const Palette& palette = m_palettes[paletteIdx];

			bool match = true;
			colourMatches.clear();

			//For each pixel
			for(int i = 0; i < tileWidth * tileHeight; i++)
			{
				int colourIdx = 0;

				//Check if this pixel colour is contained in the palette
				if(palette.GetNearestColourIdx(pixels[i], Palette::eExact, colourIdx))
				{
					colourMatches.insert(colourIdx);
				}
				else
				{
					match = false;
				}
			}

			if(match)
			{
				paletteId = paletteIdx;
				return true;
			}

			if(colourMatches.size() > closestColourCount)
			{
				//Found a closer match
				closestColourCount = colourMatches.size();
				closestPalette = paletteIdx;
			}
		}
	}

	return false;
}

bool Project::ImportPalette(Colour* pixels, Palette& palette)
{
	//Add first colour
	palette.AddColour(pixels[0]);

	//Keep running colour count
	int numColours = 1;

	const int tileWidth = GetPlatformConfig().tileWidth;
	const int tileHeight = GetPlatformConfig().tileHeight;

	//Find/add remaining colours
	for(int i = 1; i < tileWidth * tileHeight; i++)
	{
		int colourIdx = 0;
		if(!palette.GetNearestColourIdx(pixels[i], Palette::eExact, colourIdx))
		{
			if(numColours == Palette::coloursPerPalette)
			{
				//Hit the colour limit
				return false;
			}

			//Add new colour
			palette.AddColour(pixels[i]);
			numColours++;
		}
	}

	return true;
}

bool Project::MergePalettes(Palette& dest, const Palette& source)
{
	for(int i = 0; i < Palette::coloursPerPalette; i++)
	{
		if(source.IsColourUsed(i))
		{
			const Colour& sourceColour = source.GetColour(i);
			int colourIdx = 0;
			if(!dest.GetNearestColourIdx(sourceColour, Palette::eExact, colourIdx))
			{
				if(dest.AddColour(sourceColour) < 0)
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool Project::ImportBitmap(const std::string& filename, u32 importFlags, u32 paletteBits, Stamp* stamp)
{
	//Just in case
	m_tileset.RebuildHashMap();

	Map& map = m_maps[m_editingMapId];
	int mapWidth = map.GetWidth();
	int mapHeight = map.GetHeight();

	CollisionMap& collisionMap = m_collisionMaps[m_editingCollisionMapId];

	if(importFlags & eBMPImportClearPalettes)
	{
		for(int i = 0; i < s_maxPalettes; i++)
		{
			if(paletteBits & (1 << i))
			{
				m_palettes[i].Clear();
			}
		}
	}

	if(importFlags & eBMPImportClearTiles)
	{
		m_tileset.Clear();
	}

	if(importFlags & eBMPImportClearMap)
	{
		map.Clear();
	}

	if(importFlags & eBMPImportInsertBGTile)
	{
		m_tileset.AddTile();
	}

	const int tileWidth = GetPlatformConfig().tileWidth;
	const int tileHeight = GetPlatformConfig().tileHeight;

	//Read BMP
	BMPReader reader;
	if(reader.Read(filename))
	{
		if(reader.GetWidth() % tileWidth != 0 || reader.GetHeight() % tileHeight != 0)
		{
			//TODO: wx message handler in ion::debug
			//if(wxMessageBox("Bitmap width/height is not multiple of target platform tile width/height, image will be truncated", "Warning", wxOK | wxCANCEL | wxICON_WARNING) == wxCANCEL)
			{
				ion::debug::log << "Bitmap width / height " << reader.GetWidth() << "x" << reader.GetHeight() << " is not multiple of target platform tile width / height, image will be truncated: " << filename << ion::debug::end;
				return false;
			}
		}

		//Get width/height in tiles (aligned up to tile width/height)
		int tilesWidth = (int)ion::maths::Ceil((float)reader.GetWidth() / (float)tileWidth);
		int tilesHeight = (int)ion::maths::Ceil((float)reader.GetHeight() / (float)tileHeight);

		if(importFlags & eBMPImportDrawToMap)
		{
			//Grow map if necessary
			map.Resize(ion::maths::Max(mapWidth, tilesWidth), ion::maths::Max(mapHeight, tilesHeight), false, false);
			collisionMap.Resize(ion::maths::Max(collisionMap.GetWidth(), tilesWidth), ion::maths::Max(collisionMap.GetHeight(), tilesHeight), false, false);
		}

		if(importFlags & eBMPImportReplaceStamp)
		{
			if(stamp)
			{
				if((tilesWidth != stamp->GetWidth()) || (tilesHeight != stamp->GetHeight()))
				{
					//TODO: wx message handler in ion::debug
					//wxMessageBox("Bitmap width/height does not match stamp to be replaced, cannot import", "Warning", wxOK | wxICON_WARNING);
					ion::debug::log << "Bitmap width/height ("
						<< tilesWidth << "x"
						<< tilesHeight
						<< ") does not match stamp to be replaced ("
						<< stamp->GetWidth() << "x"
						<< stamp->GetHeight()
						<< "), resizing: " << filename << ion::debug::end;

					stamp->Resize(tilesWidth, tilesHeight, false, false);
				}
			}
		}

		if(importFlags & eBMPImportToStamp)
		{
			if(!stamp)
			{
				//Create new stamp
				StampId stampId = AddStamp(tilesWidth, tilesHeight);
				stamp = GetStamp(stampId);
			}
		}

		if((importFlags & eBMPImportToStamp) || (importFlags & eBMPImportReplaceStamp))
		{
			if(stamp)
			{
				//Set name
				std::string stampName = filename;

				const size_t lastSlash = stampName.find_last_of('\\');
				if(std::string::npos != lastSlash)
				{
					stampName.erase(0, lastSlash + 1);
				}

				// Remove extension if present.
				const size_t period = stampName.rfind('.');
				if(std::string::npos != period)
				{
					stampName.erase(period);
				}

				stamp->SetName(stampName);
			}
		}

		int paletteIndex = -1;

		if(importFlags & eBMPImportWholePalette)
		{
			//Read whole palette
			for(int i = 0; i < s_maxPalettes && paletteIndex == -1; i++)
			{
				if(paletteBits & (1 << i))
				{
					paletteIndex = i;
				}
			}

			//Insert palettes up to this index
			while(paletteIndex >= m_palettes.size())
			{
				m_palettes.push_back(Palette());
			}

			//Clear palette
			m_palettes[paletteIndex] = Palette();

			//Get palette
			Palette& palette = m_palettes[paletteIndex];

			for(int i = 0; i < reader.GetPaletteSize(); i++)
			{
				BMPReader::Colour bmpColour = reader.GetPaletteEntry(i);
				palette.AddColour(Colour(bmpColour.r, bmpColour.g, bmpColour.b));
			}
		}

		//For all 8x8 tiles, in row-at-a-time order
		for(int tileY = 0; tileY < tilesHeight; tileY++)
		{
			for(int tileX = 0; tileX < tilesWidth; tileX++)
			{
				Tile tile(m_platformConfig.tileWidth, m_platformConfig.tileHeight);
				PaletteId paletteId = (importFlags & eBMPImportWholePalette) ? paletteIndex : 0;

				if(importFlags & eBMPImportWholePalette)
				{
					//Copy colour indices directly
					for(int pixelX = 0; pixelX < tileWidth; pixelX++)
					{
						for(int pixelY = 0; pixelY < tileHeight; pixelY++)
						{
							int sourcePixelX = (tileX * tileWidth) + pixelX;
							int sourcePixelY = (tileY * tileHeight) + pixelY;
							tile.SetPixelColour(pixelX, pixelY, reader.GetColourIndex(sourcePixelX, sourcePixelY));
						}
					}
				}
				else
				{
					//Search for colours
					std::vector<Colour> pixels(tileWidth * tileHeight);

					//Read pixel colours from bitmap
					for(int pixelX = 0; pixelX < tileWidth; pixelX++)
					{
						for(int pixelY = 0; pixelY < tileHeight; pixelY++)
						{
							int sourcePixelX = (tileX * tileWidth) + pixelX;
							int sourcePixelY = (tileY * tileHeight) + pixelY;

							if(sourcePixelX < reader.GetWidth() && sourcePixelY < reader.GetHeight())
							{
								BMPReader::Colour bmpColour = reader.GetPixel(sourcePixelX, sourcePixelY);
								pixels[(pixelY * tileWidth) + pixelX] = Colour(bmpColour.r, bmpColour.g, bmpColour.b);
							}
							else
							{
								BMPReader::Colour bmpColour = reader.GetPaletteEntry(0);
								pixels[(pixelY * tileWidth) + pixelX] = Colour(bmpColour.r, bmpColour.g, bmpColour.b);
							}
						}
					}

					//Find or create palette
					PaletteId closestPaletteId = 0;
					int closestPaletteColourMatches = 0;
					if(!FindPalette(pixels.data(), paletteBits, paletteId, closestPaletteId, closestPaletteColourMatches))
					{
						//Import palette
						Palette importedPalette;
						if(!ImportPalette(pixels.data(), importedPalette))
						{
							//TODO: wx message handler in ion::debug
							//wxMessageBox("Too many colours in tile, bailing out", "Error", wxOK | wxICON_ERROR);
							ion::debug::log << "Too many colours in tile, bailing out: " << filename << ion::debug::end;
							return false;
						}

						//If closest palette has enough space to merge
						bool merged = false;

						int closestPaletteUsedColours = 0;
						int importedPaletteUsedColours = 0;

						for(int i = 0; i < Palette::coloursPerPalette; i++)
						{
							if(m_palettes[closestPaletteId].IsColourUsed(i))
							{
								closestPaletteUsedColours++;
							}
						}

						for(int i = 0; i < Palette::coloursPerPalette; i++)
						{
							if(importedPalette.IsColourUsed(i))
							{
								importedPaletteUsedColours++;
							}
						}

						int spareColours = Palette::coloursPerPalette - closestPaletteUsedColours;
						int requiredNewColours = importedPaletteUsedColours - closestPaletteColourMatches;

						if(spareColours >= requiredNewColours)
						{
							//Merge palettes
							if(MergePalettes(m_palettes[closestPaletteId], importedPalette))
							{
								paletteId = closestPaletteId;
								merged = true;
							}
						}

						if(!merged)
						{
							//Find free useable palette
							bool foundFreePalette = false;
							for(int i = 0; i < s_maxPalettes && !foundFreePalette; i++)
							{
								if((paletteBits & (1 << i)) && m_palettes[i].GetUsedColourMask() == 0)
								{
									paletteId = i;
									foundFreePalette = true;
								}
							}

							if(!foundFreePalette)
							{
								//TODO: wx message handler in ion::debug
								//wxMessageBox("Exceeded palette count, bailing out", "Error", wxOK | wxICON_ERROR);
								ion::debug::log << "Exceeded palette count, bailing out: " << filename << ion::debug::end;
								return false;
							}

							//Use imported palette
							m_palettes[paletteId] = importedPalette;
						}
					}

					//Get palette
					Palette& palette = m_palettes[paletteId];

					//Find pixel colours from palette
					for(int pixelX = 0; pixelX < tileWidth; pixelX++)
					{
						for(int pixelY = 0; pixelY < tileHeight; pixelY++)
						{
							int colourIdx = 0;
							if(!palette.GetNearestColourIdx(pixels[(pixelY * tileWidth) + pixelX], Palette::eExact, colourIdx))
							{
								//Shouldn't reach here - palette should have been validated
								//TODO: wx message handler in ion::debug
								//wxMessageBox("Error mapping colour indices", "Error", wxOK | wxICON_ERROR);
								ion::debug::log << "Error mapping colour indices, bailing out: " << filename << ion::debug::end;
								return false;
							}

							tile.SetPixelColour(pixelX, pixelY, colourIdx);
						}
					}
				}

				//Hash invalidated
				tile.CalculateHash();

				//Find duplicate or create new
				TileId tileId = 0;
				u32 tileFlags = 0;
				TileId duplicateId = InvalidTileId;
				
				if(!(importFlags & eBMPImportNoDuplicateTileCheck))
				{
					duplicateId = m_tileset.FindDuplicate(tile, tileFlags);
				}

				if(duplicateId != InvalidTileId)
				{
					//Tile already exists
					tileId = duplicateId;
				}
				else
				{
					//Create new tile and copy
					tileId = m_tileset.AddTile();
					Tile* newTile = m_tileset.GetTile(tileId);
					newTile->CopyPixels(tile);
					newTile->SetPaletteId(paletteId);

					//Re-add to hash map
					m_tileset.HashChanged(tileId);
				}

				if(importFlags & eBMPImportDrawToMap)
				{
					//Set in map
					map.SetTile(tileX, tileY, tileId);
					map.SetTileFlags(tileX, tileY, tileFlags);
				}

				if((importFlags & eBMPImportToStamp) || (importFlags & eBMPImportReplaceStamp))
				{
					if(stamp)
					{
						//Set in stamp
						stamp->SetTile(tileX, tileY, tileId);
						stamp->SetTileFlags(tileX, tileY, tileFlags);
					}
				}
			}
		}
	}
	else
	{
		//TODO: wx message handler in ion::debug
		//wxMessageBox("Error reading bitmap", "Error", wxOK);
		ion::debug::log << "Error reading bitmap: " << filename << ion::debug::end;
	}

	return true;
}

void Project::WriteFileHeader(std::stringstream& stream) const
{
	stream << "; == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==" << std::endl;
	stream << ";   **AUTOGENERATED WITH BEEHIVE** - the complete art tool for SEGA Mega Drive" << std::endl;
	stream << "; == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==" << std::endl;
	stream << ";   http://www.bigevilcorporation.co.uk" << std::endl;
	stream << "; == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==" << std::endl;
	stream << ";   Beehive and SEGA Genesis Framework (c) Matt Phillips 2015" << std::endl;
	stream << "; == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==" << std::endl;
	stream << std::endl;
	stream << std::endl;
}

bool Project::ExportPalettes(const std::string& filename, ExportFormat format)
{
	ion::io::File file(filename, ion::io::File::eOpenWrite);
	if(file.IsOpen())
	{
		if (format == ExportFormat::Beehive)
		{
			ion::io::Archive archive(file, ion::io::Archive::Direction::Out);
			archive.SetContentType(ion::io::Archive::Content::Minimal);
			archive.Serialise(m_paletteSlots, "palettes");
			return true;
		}
		else
		{
			std::stringstream stream;
			WriteFileHeader(stream);
			stream << "palette_" << m_name << ":" << std::endl;

			for (int i = 0; i < s_maxPalettes; i++)
			{
				m_palettes[i].Export(stream);
				stream << std::endl;
			}

			file.Write(stream.str().c_str(), stream.str().size());
		}

		return true;
	}

	return false;
}

bool Project::ExportTiles(const std::string& filename, ExportFormat format)
{
	if(format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
	{
		std::string binaryFilename = filename.substr(0, filename.find_first_of('.'));
		binaryFilename += ".bin";

		//Export binary data
		ion::io::File binaryFile(binaryFilename, ion::io::File::eOpenWrite);
		if(binaryFile.IsOpen())
		{
			m_tileset.Export(m_platformConfig, binaryFile, format == ExportFormat::BinaryCompressed);
		}
		else
		{
			return false;
		}
	}

	ion::io::File file(filename, ion::io::File::eOpenWrite);
	if(file.IsOpen())
	{
		if (format == ExportFormat::Beehive)
		{
			ion::io::Archive archive(file, ion::io::Archive::Direction::Out);
			archive.SetContentType(ion::io::Archive::Content::Minimal);
			m_tileset.Serialise(archive);
			return true;
		}
		else
		{
			std::stringstream stream;
			WriteFileHeader(stream);

			u32 binarySize = m_tileset.GetBinarySize(m_platformConfig);

			if (format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
			{
				//Export (uncompressed) size of binary file
				stream << "tiles_" << m_name << "_size_b\tequ 0x" << std::hex << std::setfill('0') << std::uppercase << std::setw(8) << binarySize << std::dec << "\t; Size in bytes" << std::endl;
			}
			else if (format == ExportFormat::Text)
			{
				//Export label, data and size as inline text
				stream << "tiles_" << m_name << ":" << std::endl;

				m_tileset.Export(m_platformConfig, stream);

				stream << std::endl;
				stream << "tiles_" << m_name << "_end" << std::endl;
				stream << "tiles_" << m_name << "_size_b\tequ (tiles_" << m_name << "_end-tiles_" << m_name << ")\t; Size in bytes" << std::endl;
			}
			else
			{
				ion::debug::Log("Unsupported export format");
			}

			stream << "tiles_" << m_name << "_size_w\tequ (tiles_" << m_name << "_size_b/2)\t; Size in words" << std::endl;
			stream << "tiles_" << m_name << "_size_l\tequ (tiles_" << m_name << "_size_b/4)\t; Size in longwords" << std::endl;
			stream << "tiles_" << m_name << "_size_t\tequ " << m_tileset.GetCount() << "\t; Size in tiles" << std::endl;

			file.Write(stream.str().c_str(), stream.str().size());
		}

		return true;
	}

	return false;
}

bool Project::ExportTerrainTiles(const std::string& filename, ExportFormat format)
{
	if (format == ExportFormat::Beehive)
	{
		ion::io::File file(filename, ion::io::File::eOpenWrite);
		if (file.IsOpen())
		{
			ion::io::Archive archive(file, ion::io::Archive::Direction::Out);
			archive.SetContentType(ion::io::Archive::Content::Minimal);
			m_terrainTileset.Serialise(archive);
			return true;
		}
	}
	else
	{
		u32 binarySize = 0;

		if (format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
		{
			std::string binaryFilename = filename.substr(0, filename.find_first_of('.'));
			binaryFilename += ".bin";

			//Export binary data
			ion::io::File binaryFile(binaryFilename, ion::io::File::eOpenWrite);
			if (binaryFile.IsOpen())
			{
				m_terrainTileset.Export(binaryFile);
				binarySize = binaryFile.GetSize();
			}
			else
			{
				return false;
			}
		}

		ion::io::File file(filename, ion::io::File::eOpenWrite);
		if (file.IsOpen())
		{
			std::stringstream stream;
			WriteFileHeader(stream);

			if (format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
			{
				//Export size of binary file
				stream << "TerrainTiles_" << m_name << "_size_b\tequ 0x" << std::hex << std::setfill('0') << std::uppercase << std::setw(8) << binarySize << std::dec << "\t; Size in bytes" << std::endl;
			}
			else if (format == ExportFormat::Text)
			{
				//Export label, data and size as inline text
				stream << "TerrainTiles_" << m_name << ":" << std::endl;

				m_terrainTileset.Export(stream);

				stream << std::endl;
				stream << "TerrainTiles_" << m_name << "_end" << std::endl;
				stream << "TerrainTiles_" << m_name << "_size_b\tequ (TerrainTiles_" << m_name << "_end-TerrainTiles_" << m_name << ")\t; Size in bytes" << std::endl;
			}
			else
			{
				ion::debug::Log("Unsupported export format");
			}

			stream << "TerrainTiles_" << m_name << "_size_w\tequ (TerrainTiles_" << m_name << "_size_b/2)\t; Size in words" << std::endl;
			stream << "TerrainTiles_" << m_name << "_size_l\tequ (TerrainTiles_" << m_name << "_size_b/4)\t; Size in longwords" << std::endl;
			stream << "TerrainTiles_" << m_name << "_size_t\tequ " << m_terrainTileset.GetCount() << "\t; Size in tiles" << std::endl;

			file.Write(stream.str().c_str(), stream.str().size());

			return true;
		}
	}

	return false;
}

bool Project::ExportTerrainAngles(const std::string& filename, ExportFormat format)
{
	std::string binaryFilename = filename.substr(0, filename.find_first_of('.'));
	binaryFilename += ".bin";

	//Export binary data
	ion::io::File binaryFile(binaryFilename, ion::io::File::eOpenWrite);
	if (binaryFile.IsOpen())
	{
		m_terrainTileset.ExportAngles(binaryFile);
		return true;
	}

	return false;
}

bool Project::ExportMap(MapId mapId, const std::string& filename, ExportFormat format) const
{
	const Map& map = m_maps.find(mapId)->second;

	if (format == ExportFormat::Beehive)
	{
		ion::io::File file(filename, ion::io::File::eOpenWrite);
		if (file.IsOpen())
		{
			//Bake map/stamps to tiles
			std::vector<Map::TileDesc> tiles;
			ion::Vector2i mapSize;

			map.Export(*this, tiles);

			mapSize.x = map.GetWidth();
			mapSize.y = map.GetHeight();

			ion::io::Archive archive(file, ion::io::Archive::Direction::Out);
			archive.SetContentType(ion::io::Archive::Content::Minimal);
			archive.Serialise(mapSize, "size");
			archive.Serialise(tiles, "tileMap");

			return true;
		}
	}
	else
	{
		int mapWidth = map.GetWidth();
		int mapHeight = map.GetHeight();
		const std::string& mapName = map.GetName();

		u32 binarySize = 0;

		if (format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
		{
			std::string binaryFilename = filename.substr(0, filename.find_first_of('.'));
			binaryFilename += ".bin";

			//Export binary data
			ion::io::File binaryFile(binaryFilename, ion::io::File::eOpenWrite);
			if (binaryFile.IsOpen())
			{
				map.Export(*this, binaryFile);
				binarySize = binaryFile.GetSize();
			}
			else
			{
				return false;
			}
		}

		ion::io::File file(filename, ion::io::File::eOpenWrite);
		if (file.IsOpen())
		{
			std::stringstream stream;
			WriteFileHeader(stream);

			if (format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
			{
				//Export size of binary file
				stream << "map_" << mapName << "_size_b\tequ 0x" << std::hex << std::setfill('0') << std::uppercase << std::setw(8) << binarySize << std::dec << "\t; Size in bytes" << std::endl;
			}
			else if (format == ExportFormat::Text)
			{
				//Export label, data and size as inline text
				stream << "map_" << mapName << ":" << std::endl;

				map.Export(*this, stream);

				stream << std::endl;
				stream << "map_" << mapName << "_end:" << std::endl;
				stream << "map_" << mapName << "_size_b\tequ (map_" << mapName << "_end-map_" << mapName << ")\t; Size in bytes" << std::endl;
			}
			else
			{
				ion::debug::Log("Unsupported export format");
			}

			stream << "map_" << mapName << "_size_w\tequ (map_" << mapName << "_size_b/2)\t; Size in words" << std::endl;
			stream << "map_" << mapName << "_size_l\tequ (map_" << mapName << "_size_b/4)\t; Size in longwords" << std::endl;

			stream << std::hex << std::setfill('0') << std::uppercase;
			stream << "map_" << mapName << "_width\tequ " << "0x" << std::setw(2) << mapWidth << std::endl;
			stream << "map_" << mapName << "_height\tequ " << "0x" << std::setw(2) << mapHeight << std::endl;
			stream << std::dec;

			file.Write(stream.str().c_str(), stream.str().size());

			return true;
		}
	}

	return false;
}

bool Project::ExportBlocks(const std::string& filename, ExportFormat format, int blockWidth, int blockHeight)
{
	//Generate map blocks and concatenate into one list
	std::vector<Map::Block*> blocks;
	for(TMapMap::iterator it = m_maps.begin(), end = m_maps.end(); it != end; ++it)
	{
		it->second.GenerateBlocks(*this, blockWidth, blockHeight);

		std::vector<Map::Block>& currBlocks = it->second.GetBlocks();
		blocks.reserve(blocks.size() + currBlocks.size());
		for(int i = 0; i < currBlocks.size(); i++)
		{
			blocks.push_back(&currBlocks[i]);
		}
	}

	//Find unique blocks
	std::vector<Map::Block*> uniqueBlocks;

	for(int i = 0; i < blocks.size(); i++)
	{
		Map::Block* blockA = blocks[i];

		if(blocks[i]->uniqueIndex == -1)
		{
			blocks[i]->uniqueIndex = uniqueBlocks.size();
			blocks[i]->unique = true;

			uniqueBlocks.push_back(blocks[i]);

			for(int j = i + 1; j < blocks.size(); j++)
			{
				Map::Block* blockB = blocks[j];

				bool equal = true;

				if(!(*blocks[i] == *blocks[j]))
				{
					//Solid (all pixels same colour) tiles are considered equivalent regardless of flip flags
					for(int k = 0; k < blocks[i]->m_tiles.size() && equal; k++)
					{
						TileId tileIdA = blocks[i]->m_tiles[k].m_id;
						TileId tileIdB = blocks[j]->m_tiles[k].m_id;
						u32 tileFlagsA = blocks[i]->m_tiles[k].m_flags;
						u32 tileFlagsB = blocks[j]->m_tiles[k].m_flags;

						//If tile ids and flags match, tile is definitely equal
						if(tileIdA != tileIdB || tileFlagsA != tileFlagsB)
						{
							//Check tile A == tile B without taking id or flip flags into account
							const Tile* tileA = m_tileset.GetTile(blocks[i]->m_tiles[k].m_id);
							const Tile* tileB = m_tileset.GetTile(blocks[j]->m_tiles[k].m_id);

							if(*tileA == *tileB)
							{
								//Check if tile is solid colour
								bool solidColourTile = true;

								u8 firstColourIdx = tileA->GetPixelColour(0, 0);

								for(int tileX = 0; tileX < m_platformConfig.tileWidth && solidColourTile; tileX++)
								{
									for(int tileY = 0; tileY < m_platformConfig.tileHeight && solidColourTile; tileY++)
									{
										solidColourTile = (firstColourIdx == tileA->GetPixelColour(tileX, tileY));
									}
								}

								if (solidColourTile)
								{
									//Solid colour, only priority flags must match
									equal = ((tileFlagsA & Map::eHighPlane) == (tileFlagsB & Map::eHighPlane));
								}
								else
								{
									//Not solid colour, all flags must match
									equal = (tileFlagsA == tileFlagsB);
								}
							}
							else
							{
								//Tiles don't match
								equal = false;
							}
						}
					}
				}

				if(equal)
				{
					blocks[j]->uniqueIndex = blocks[i]->uniqueIndex;
				}
			}
		}
	}

	//m_blocksInvalidated = true;

	u32 binarySize = 0;

	if(format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
	{
		std::string binaryFilename = filename.substr(0, filename.find_first_of('.'));
		if(binaryFilename.size() > 0)
		{
			binaryFilename += ".bin";

			//Export binary data
			ion::io::File binaryFile(binaryFilename, ion::io::File::eOpenWrite);
			if(binaryFile.IsOpen())
			{
				for(int i = 0; i < uniqueBlocks.size(); i++)
				{
					uniqueBlocks[i]->Export(*this, binaryFile, blockWidth, blockHeight);
				}

				binarySize = binaryFile.GetSize();
			}
			else
			{
				return false;
			}
		}
	}

	ion::io::File file(filename, ion::io::File::eOpenWrite);
	if(file.IsOpen())
	{
		std::stringstream stream;
		WriteFileHeader(stream);

		if(format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
		{
			//Export size of binary file
			stream << "map_blocks_" << m_name << "_size_b\tequ 0x" << std::hex << std::setfill('0') << std::uppercase << std::setw(8) << binarySize << std::dec << "\t; Size in bytes" << std::endl;
		}
		else if (format == ExportFormat::Text)
		{
			//Export label, data and size as inline text
			stream << "map_blocks_" << m_name << ":" << std::endl;
		
			for(int i = 0; i < uniqueBlocks.size(); i++)
			{
				uniqueBlocks[i]->Export(*this, stream, blockWidth, blockHeight);
			}
		
			stream << std::endl;
			stream << "map_blocks_" << m_name << "_end:" << std::endl;
			stream << "map_blocks_" << m_name << "_size_b\tequ (map_blocks_" << m_name << "_end-map_" << m_name << ")\t; Size in bytes" << std::endl;
		}
		else
		{
			ion::debug::Log("Unsupported export format");
		}

		stream << "map_blocks_" << m_name << "_size_w\tequ (map_blocks_" << m_name << "_size_b/2)\t; Size in words" << std::endl;
		stream << "map_blocks_" << m_name << "_size_l\tequ (map_blocks_" << m_name << "_size_b/4)\t; Size in longwords" << std::endl;
		stream << "map_blocks_" << m_name << "_num_blocks\tequ " << uniqueBlocks.size() << "\t; Size in blocks" << std::endl;

		file.Write(stream.str().c_str(), stream.str().size());

		return true;
	}

	return false;
}

bool Project::ExportBlockMap(MapId mapId, const std::string& filename, ExportFormat format, int blockWidth, int blockHeight) const
{
	const Map& map = m_maps.find(mapId)->second;
	int mapWidth = map.GetBlockAlignedWidth(blockWidth);
	int mapHeight = map.GetBlockAlignedHeight(blockHeight);
	const std::string& mapName = map.GetName();

	u32 binarySize = 0;
	std::vector<u32> colOffsets;

	if(format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
	{
		std::string binaryFilename = filename.substr(0, filename.find_first_of('.'));
		binaryFilename += ".bin";

		//Export binary data
		ion::io::File binaryFile(binaryFilename, ion::io::File::eOpenWrite);
		if(binaryFile.IsOpen())
		{
			map.ExportBlockMap(*this, binaryFile, blockWidth, blockHeight, colOffsets);
			binarySize = binaryFile.GetSize();
		}
		else
		{
			return false;
		}
	}

	ion::io::File file(filename, ion::io::File::eOpenWrite);
	if(file.IsOpen())
	{
		std::stringstream stream;
		WriteFileHeader(stream);

		if(format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
		{
			//Export size of binary file
			stream << "map_blockmap_" << mapName << "_size_b\tequ 0x" << std::hex << std::setfill('0') << std::uppercase << std::setw(8) << binarySize << std::dec << "\t; Size in bytes" << std::endl;
		}
		else if (format == ExportFormat::Text)
		{
			//Export label, data and size as inline text
			stream << "map_blockmap_" << mapName << ":" << std::endl;

			map.ExportBlockMap(*this, stream, blockWidth, blockHeight);

			stream << std::endl;
			stream << "map_blockmap_" << mapName << "_end:" << std::endl;
			stream << "map_blockmap_" << mapName << "_size_b\tequ (map_blockmap_" << mapName << "_end-map_" << mapName << ")\t; Size in bytes" << std::endl;
		}
		else
		{
			ion::debug::Log("Unsupported export format");
		}

		stream << "map_blockmap_" << mapName << "_size_w\tequ (map_blockmap_" << mapName << "_size_b/2)\t; Size in words" << std::endl;
		stream << "map_blockmap_" << mapName << "_size_l\tequ (map_blockmap_" << mapName << "_size_b/4)\t; Size in longwords" << std::endl;

		stream << std::hex << std::setfill('0') << std::uppercase;
		stream << "map_" << mapName << "_width\tequ " << "0x" << std::setw(2) << mapWidth << std::endl;
		stream << "map_" << mapName << "_height\tequ " << "0x" << std::setw(2) << mapHeight << std::endl;
		stream << "map_blockmap_" << mapName << "_width\tequ " << "0x" << std::setw(2) << map.GetWidthBlocks(blockWidth) << std::endl;
		stream << "map_blockmap_" << mapName << "_height\tequ " << "0x" << std::setw(2) << map.GetHeightBlocks(blockHeight) << std::endl;
		stream << std::endl;

		stream << "map_blockmap_" << mapName << "_coloffsets:" << std::endl;

		//Export column offsets
		for(int i = 0; i < colOffsets.size(); i++)
		{
			stream << "\tdc.w " << "0x" << HEX4(colOffsets[i]) << std::endl;
		}

		stream << std::dec;

		file.Write(stream.str().c_str(), stream.str().size());

		return true;
	}

	return false;
}

bool Project::ExportTerrainBlocks(const std::string& filename, ExportFormat format, int blockWidth, int blockHeight)
{
	//Generate map blocks and concatenate into one list
	std::vector<CollisionMap::Block*> blocks;
	for(TCollisionMapMap::iterator it = m_collisionMaps.begin(), end = m_collisionMaps.end(); it != end; ++it)
	{
		if(it->second.GetNumTerrainBeziers() > 0)
		{
			it->second.GenerateBlocks(*this, blockWidth, blockHeight);

			std::vector<CollisionMap::Block>& currBlocks = it->second.GetBlocks();
			blocks.reserve(blocks.size() + currBlocks.size());
			for(int i = 0; i < currBlocks.size(); i++)
			{
				blocks.push_back(&currBlocks[i]);
			}
		}
	}

	//Find unique blocks
	std::vector<CollisionMap::Block*> uniqueBlocks;

	for(int i = 0; i < blocks.size(); i++)
	{
		if(blocks[i]->uniqueIndex == -1)
		{
			blocks[i]->uniqueIndex = uniqueBlocks.size();
			uniqueBlocks.push_back(blocks[i]);

			for(int j = i + 1; j < blocks.size(); j++)
			{
				if(*blocks[i] == *blocks[j])
				{
					blocks[j]->uniqueIndex = blocks[i]->uniqueIndex;
				}
			}
		}
	}

	u32 binarySize = 0;

	if(format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
	{
		std::string binaryFilename = filename.substr(0, filename.find_first_of('.'));
		binaryFilename += ".bin";

		//Export binary data
		ion::io::File binaryFile(binaryFilename, ion::io::File::eOpenWrite);
		if(binaryFile.IsOpen())
		{
			for(int i = 0; i < uniqueBlocks.size(); i++)
			{
				uniqueBlocks[i]->Export(*this, binaryFile, blockWidth, blockHeight);
			}

			binarySize = binaryFile.GetSize();
		}
		else
		{
			return false;
		}
	}

	ion::io::File file(filename, ion::io::File::eOpenWrite);
	if(file.IsOpen())
	{
		std::stringstream stream;
		WriteFileHeader(stream);

		if(format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
		{
			//Export size of binary file
			stream << "terrainmap_blocks_" << m_name << "_size_b\tequ 0x" << std::hex << std::setfill('0') << std::uppercase << std::setw(8) << binarySize << std::dec << "\t; Size in bytes" << std::endl;
		}
		else if (format == ExportFormat::Text)
		{
			//Export label, data and size as inline text
			stream << "terrainmap_blocks_" << m_name << ":" << std::endl;

			for(int i = 0; i < uniqueBlocks.size(); i++)
			{
				uniqueBlocks[i]->Export(*this, stream, blockWidth, blockHeight);
			}

			stream << std::endl;
			stream << "terrainmap_blocks_" << m_name << "_end:" << std::endl;
			stream << "terrainmap_blocks_" << m_name << "_size_b\tequ (terrainmap_blocks_" << m_name << "_end-terrainmap_" << m_name << ")\t; Size in bytes" << std::endl;
		}
		else
		{
			ion::debug::Log("Unsupported export format");
		}

		stream << "terrainmap_blocks_" << m_name << "_size_w\tequ (terrainmap_blocks_" << m_name << "_size_b/2)\t; Size in words" << std::endl;
		stream << "terrainmap_blocks_" << m_name << "_size_l\tequ (terrainmap_blocks_" << m_name << "_size_b/4)\t; Size in longwords" << std::endl;
		stream << "terrainmap_blocks_" << m_name << "_num_blocks\tequ " << uniqueBlocks.size() << "\t; Size in blocks" << std::endl;

		file.Write(stream.str().c_str(), stream.str().size());

		return true;
	}

	return false;
}

bool Project::ExportTerrainBlockMap(MapId mapId, const std::string& filename, ExportFormat format, int blockWidth, int blockHeight)
{
	const Map& map = m_maps.find(mapId)->second;
	CollisionMap& collisionMap = m_collisionMaps.find(mapId)->second;
	const int tileWidth = GetPlatformConfig().tileWidth;
	const int tileHeight = GetPlatformConfig().tileHeight;
	const int mapHeightPixels = (collisionMap.GetHeight() * tileHeight);
	const std::string& mapName = map.GetName();

	ion::Vector2i topLeftBlocks;
	ion::Vector2i sizeBlocks;
	collisionMap.CalculatePhysicsWorldBoundsBlocks(topLeftBlocks, sizeBlocks, tileWidth, tileHeight, blockWidth, blockHeight);

	ion::Vector2i topLeftTiles = topLeftBlocks * ion::Vector2i(blockWidth, blockHeight);
	ion::Vector2i sizeTiles = sizeBlocks * ion::Vector2i(blockWidth, blockHeight);

	if(collisionMap.GetNumTerrainBeziers() > 0)
	{
		u32 binarySize = 0;
		std::vector<u16> rowOffsets;

		if(format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
		{
			std::string binaryFilename = filename.substr(0, filename.find_first_of('.'));
			binaryFilename += ".bin";

			//Export binary data
			ion::io::File binaryFile(binaryFilename, ion::io::File::eOpenWrite);
			if(binaryFile.IsOpen())
			{
				collisionMap.ExportBlockMap(*this, binaryFile, blockWidth, blockHeight, rowOffsets);
				binarySize = binaryFile.GetSize();
			}
			else
			{
				return false;
			}
		}

		ion::io::File file(filename, ion::io::File::eOpenWrite);
		if(file.IsOpen())
		{
			std::stringstream stream;
			WriteFileHeader(stream);

			if(format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
			{
				//Export size of binary file
				stream << "terrainmap_blockmap_" << mapName << "_size_b\tequ 0x" << std::hex << std::setfill('0') << std::uppercase << std::setw(8) << binarySize << std::dec << "\t; Size in bytes" << std::endl;
			}
			else if (format == ExportFormat::Text)
			{
				//Export label, data and size as inline text
				stream << "terrainmap_blockmap_" << mapName << ":" << std::endl;

				collisionMap.ExportBlockMap(*this, stream, blockWidth, blockHeight);

				stream << std::endl;
				stream << "terrainmap_blockmap_" << mapName << "_end:" << std::endl;
				stream << "terrainmap_blockmap_" << mapName << "_size_b\tequ (terrainmap_blockmap_" << mapName << "_end-terrainmap_" << mapName << ")\t; Size in bytes" << std::endl;
			}
			else
			{
				ion::debug::Log("Unsupported export format");
			}

			stream << "terrainmap_blockmap_" << mapName << "_size_w\tequ (terrainmap_blockmap_" << mapName << "_size_b/2)\t; Size in words" << std::endl;
			stream << "terrainmap_blockmap_" << mapName << "_size_l\tequ (terrainmap_blockmap_" << mapName << "_size_b/4)\t; Size in longwords" << std::endl;

			stream << std::hex << std::setfill('0') << std::uppercase;
			stream << "terrainmap_" << mapName << "_left\tequ " << "0x" << std::setw(2) << topLeftTiles.x << std::endl;
			stream << "terrainmap_" << mapName << "_top\tequ " << "0x" << std::setw(2) << topLeftTiles.y << std::endl;
			stream << "terrainmap_" << mapName << "_width\tequ " << "0x" << std::setw(2) << sizeTiles.x << std::endl;
			stream << "terrainmap_" << mapName << "_height\tequ " << "0x" << std::setw(2) << sizeTiles.y << std::endl;
			stream << "terrainmap_blockmap_" << mapName << "_left\tequ " << "0x" << std::setw(2) << topLeftBlocks.x << std::endl;
			stream << "terrainmap_blockmap_" << mapName << "_top\tequ " << "0x" << std::setw(2) << topLeftBlocks.y << std::endl;
			stream << "terrainmap_blockmap_" << mapName << "_width\tequ " << "0x" << std::setw(2) << sizeBlocks.x << std::endl;
			stream << "terrainmap_blockmap_" << mapName << "_height\tequ " << "0x" << std::setw(2) << sizeBlocks.y << std::endl;
			stream << std::dec;
			stream << std::endl;

			//Export row offsets
			stream << "collisionmap_blockmap_yoffs_" << mapName << ":" << std::endl;

			for (int i = 0; i < rowOffsets.size(); i++)
			{
				stream << "\tdc.w " << "0x" << HEX4(rowOffsets[i]) << std::endl;
			}

			stream << std::endl;

			//Export bezier metadata for 'special' terrain
			std::vector<std::tuple<ion::Vector2i, ion::Vector2i, u16>> specialTerrainStartEndPositions;
			ion::Vector2 boundsMin;
			ion::Vector2 boundsMax;

			for(int i = 0; i < collisionMap.GetNumTerrainBeziers(); i++)
			{
				if(collisionMap.GetTerrainBezierFlags(i) & (eCollisionTileFlagWater | eCollisionTileFlagSpecial))
				{
					if(const ion::gamekit::BezierPath* path = collisionMap.GetTerrainBezier(i))
					{
						if(path->GetNumPoints() >= 2)
						{
							path->GetBounds(boundsMin, boundsMax);
							specialTerrainStartEndPositions.push_back(std::make_tuple(ion::Vector2i(boundsMin.x, mapHeightPixels - 1 - boundsMax.y), ion::Vector2i(boundsMax.x, mapHeightPixels - 1 - boundsMin.y), collisionMap.GetTerrainBezierFlags(i)));
						}
					}
				}
			}

			stream << "; Terrain bezier bounds" << std::endl;
			stream << "terrainmap_" << mapName << "_num_special_terrain_descs\tequ " << "0x" << std::setw(2) << specialTerrainStartEndPositions.size() << std::endl;
			stream << "terrainmap_" << mapName << "_special_terrain_descs:" << std::endl;

			for(int i = 0; i < specialTerrainStartEndPositions.size(); i++)
			{
				stream << "\tdc.w "
					<< "0x" << HEX4(std::get<0>(specialTerrainStartEndPositions[i]).x) << ", "
					<< "0x" << HEX4(std::get<0>(specialTerrainStartEndPositions[i]).y) << ", "
					<< "0x" << HEX4(std::get<1>(specialTerrainStartEndPositions[i]).x) << ", "
					<< "0x" << HEX4(std::get<1>(specialTerrainStartEndPositions[i]).y) << ", "
					<< "0x" << HEX4(std::get<2>(specialTerrainStartEndPositions[i])) << "\t; Left, top, right, bottom, flags" << std::endl;
			}

			stream << std::endl;

			file.Write(stream.str().c_str(), stream.str().size());

			return true;
		}
	}

	return false;
}

bool Project::ExportStamps(const std::string& filename, ExportFormat format)
{
	if (format == ExportFormat::Beehive)
	{
		ion::io::File file(filename, ion::io::File::eOpenWrite);
		if (file.IsOpen())
		{
			ion::io::Archive archive(file, ion::io::Archive::Direction::Out);
			archive.SetContentType(ion::io::Archive::Content::Minimal);
			archive.Serialise(m_stamps, "stamps");
			return true;
		}
	}
	else
	{
		u32 binarySize = 0;

#pragma pack(push, 1)
		struct StampTableEntry
		{
			StampTableEntry() {}
			StampTableEntry(u32 offset, u8 width, u8 height)
			{
				m_offset = offset;
				m_width = width;
				m_height = height;
			}

			u32 m_offset;
			u16 m_width;
			u16 m_height;
		};
#pragma pack(pop)

		std::vector<StampTableEntry> stampTable;

		if (format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
		{
			std::string binaryFilename = filename.substr(0, filename.find_first_of('.'));
			binaryFilename += ".bin";

			//Export binary data
			ion::io::File binaryFile(binaryFilename, ion::io::File::eOpenWrite);
			if (binaryFile.IsOpen())
			{
				for (TStampMap::const_iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
				{
					stampTable.push_back(StampTableEntry(binaryFile.GetSize(), it->second.GetWidth(), it->second.GetHeight()));
					it->second.Export(*this, binaryFile);
				}

				binarySize = binaryFile.GetSize();
			}
			else
			{
				return false;
			}
		}

		ion::io::File file(filename, ion::io::File::eOpenWrite);
		if (file.IsOpen())
		{
			std::stringstream stream;
			WriteFileHeader(stream);

			if (format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
			{
				//Export size of binary file
				stream << "stamps_" << m_name << "_size_b\tequ 0x" << HEX8(binarySize) << "\t; Binary size in bytes" << std::endl;

				stream << std::endl;

				//Export stamp table
				stream << "stamps_" << m_name << "_table:" << std::endl;
				stream << std::endl;

				for (int i = 0; i < stampTable.size(); i++)
				{
					stream << "stamp_" << i << "_offset:\tdc.l 0x" << HEX8((int)stampTable[i].m_offset) << std::endl;
					stream << "stamp_" << i << "_width:\tdc.w 0x" << HEX4((int)stampTable[i].m_width) << std::endl;
					stream << "stamp_" << i << "_height:\tdc.w 0x" << HEX4((int)stampTable[i].m_height) << std::endl;
					stream << std::endl;
				}
			}
			else if (format == ExportFormat::Text)
			{
				//Export label, data and size as inline text
				stream << "stamps_" << m_name << ":" << std::endl;

				int offset = 0;
				for (TStampMap::const_iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
				{
					stampTable.push_back(StampTableEntry(offset, it->second.GetWidth(), it->second.GetHeight()));
					stream << "stamp_" << it->first << ":" << std::endl;
					it->second.Export(*this, stream);
					stream << std::endl;

					offset += it->second.GetBinarySize();
				}

				//Export stamp table
				stream << "stamps_" << m_name << "_table:" << std::endl;
				stream << std::endl;

				for (int i = 0; i < stampTable.size(); i++)
				{
					stream << "stamp_" << i << "_offset:\tdc.l 0x" << HEX8((int)stampTable[i].m_offset) << std::endl;
					stream << "stamp_" << i << "_width:\tdc.w 0x" << HEX4((int)stampTable[i].m_width) << std::endl;
					stream << "stamp_" << i << "_height:\tdc.w 0x" << HEX4((int)stampTable[i].m_height) << std::endl;
					stream << std::endl;
				}

				stream << std::endl;
				stream << "stamps_" << m_name << "_end:" << std::endl;
				stream << "stamps_" << m_name << "_size_b\tequ (stamps_" << m_name << "_end-stamps_" << m_name << ")\t; Size in bytes" << std::endl;
			}

			stream << "stamps_" << m_name << "_size_w\tequ (stamps_" << m_name << "_size_b/2)\t; Size in words" << std::endl;
			stream << "stamps_" << m_name << "_size_l\tequ (stamps_" << m_name << "_size_b/4)\t; Size in longwords" << std::endl;

			file.Write(stream.str().c_str(), stream.str().size());

			return true;
		}
		else
		{
			ion::debug::Log("Unsupported export format");
		}
	}

	return false;
}

bool Project::ExportStampMap(MapId mapId, const std::string& filename, ExportFormat format)
{
	Map& map = m_maps.find(mapId)->second;

	if (format == ExportFormat::Beehive)
	{
		ion::io::File file(filename, ion::io::File::eOpenWrite);
		if (file.IsOpen())
		{
			ion::io::Archive archive(file, ion::io::Archive::Direction::Out);
			archive.SetContentType(ion::io::Archive::Content::Minimal);
			ion::Vector2i mapSizeTiles(map.GetWidth(), map.GetHeight());
			archive.Serialise(mapSizeTiles, "mapSizeTiles");
			archive.Serialise(map.GetStamps(), "stamps");
			return true;
		}
	}
	else
	{
		int mapWidth = map.GetWidth();
		int mapHeight = map.GetHeight();
		const std::string& mapName = map.GetName();

		u32 binarySize = 0;

		if (format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
		{
			std::string binaryFilename = filename.substr(0, filename.find_first_of('.'));
			binaryFilename += ".bin";

			//Export binary data
			ion::io::File binaryFile(binaryFilename, ion::io::File::eOpenWrite);
			if (binaryFile.IsOpen())
			{
				map.ExportStampMap(*this, binaryFile);
				binarySize = binaryFile.GetSize();
			}
			else
			{
				return false;
			}
		}

		ion::io::File file(filename, ion::io::File::eOpenWrite);
		if (file.IsOpen())
		{
			std::stringstream stream;
			WriteFileHeader(stream);

			if (format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
			{
				//Export size of binary file
				stream << "stampmap_" << mapName << "_size_b\tequ 0x" << HEX8(binarySize) << "\t; Size in bytes" << std::endl;
			}
			else if (format == ExportFormat::Text)
			{
				//Export label, data and size as inline text
				stream << "stampmap_" << mapName << ":" << std::endl;

				map.ExportStampMap(*this, stream);

				stream << std::endl;
				stream << "stampmap_" << mapName << "_end:" << std::endl;
				stream << "stampmap_" << mapName << "_size_b\tequ (stampmap_" << mapName << "_end-stampmap_" << mapName << ")\t; Size in bytes" << std::endl;
			}
			else
			{
				ion::debug::Log("Unsupported export format");
			}

			stream << "stampmap_" << mapName << "_size_w\tequ (stampmap_" << mapName << "_size_b/2)\t; Size in words" << std::endl;
			stream << "stampmap_" << mapName << "_size_l\tequ (stampmap_" << mapName << "_size_b/4)\t; Size in longwords" << std::endl;

			stream << std::hex << std::setfill('0') << std::uppercase;
			stream << "stampmap_" << mapName << "_width\tequ " << "0x" << HEX2(mapWidth) << std::endl;
			stream << "stampmap_" << mapName << "_height\tequ " << "0x" << HEX2(mapHeight) << std::endl;
			stream << std::dec;

			file.Write(stream.str().c_str(), stream.str().size());

			return true;
		}
	}

	return false;
}

bool Project::ExportCollisionMap(MapId mapId, const std::string& filename, ExportFormat format)
{
	u32 binarySize = 0;

	const Map& map = m_maps.find(mapId)->second;
	const std::string& mapName = map.GetName();
	CollisionMap& collisionMap = m_collisionMaps.find(mapId)->second;

	if (format == ExportFormat::Beehive)
	{
		ion::io::File file(filename, ion::io::File::eOpenWrite);
		if (file.IsOpen())
		{
			ion::io::Archive archive(file, ion::io::Archive::Direction::Out);
			archive.SetContentType(ion::io::Archive::Content::Minimal);
			archive.Serialise(collisionMap, "collisionMap");
			return true;
		}
	}
	else
	{
		if (format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
		{
			std::string binaryFilename = filename.substr(0, filename.find_first_of('.'));
			binaryFilename += ".bin";

			//Export binary data
			ion::io::File binaryFile(binaryFilename, ion::io::File::eOpenWrite);
			if (binaryFile.IsOpen())
			{
				collisionMap.Export(*this, binaryFile);
				binarySize = binaryFile.GetSize();
			}
			else
			{
				return false;
			}
		}

		ion::io::File file(filename, ion::io::File::eOpenWrite);
		if (file.IsOpen())
		{
			std::stringstream stream;
			WriteFileHeader(stream);

			if (format == ExportFormat::Binary || format == ExportFormat::BinaryCompressed)
			{
				//Export size of binary file
				stream << "collisionmap_" << mapName << "_size_b\tequ 0x" << std::hex << std::setfill('0') << std::uppercase << std::setw(8) << binarySize << std::dec << "\t; Size in bytes" << std::endl;
			}
			else if (format == ExportFormat::Text)
			{
				//Export label, data and size as inline text
				stream << "collisionmap_" << mapName << ":" << std::endl;

				collisionMap.Export(*this, stream);

				stream << std::endl;
				stream << "collisionmap_" << mapName << "_end:" << std::endl;
				stream << "collisionmap_" << mapName << "_size_b\tequ (collisionmap_" << mapName << "_end-collisionmap_" << mapName << ")\t; Size in bytes" << std::endl;
			}
			else
			{
				ion::debug::Log("Unsupported export format");
			}

			stream << "collisionmap_" << mapName << "_size_w\tequ (collisionmap_" << mapName << "_size_b/2)\t; Size in words" << std::endl;
			stream << "collisionmap_" << mapName << "_size_l\tequ (collisionmap_" << mapName << "_size_b/4)\t; Size in longwords" << std::endl;

			stream << std::hex << std::setfill('0') << std::uppercase;
			stream << "collisionmap_" << mapName << "_width\tequ " << "0x" << std::setw(2) << collisionMap.GetWidth() << std::endl;
			stream << "collisionmap_" << mapName << "_height\tequ " << "0x" << std::setw(2) << collisionMap.GetHeight() << std::endl;
			stream << std::dec;

			file.Write(stream.str().c_str(), stream.str().size());

			return true;
		}
	}

	return false;
}

namespace
{
	std::string GenerateObjectName(const std::string& mapName, const GameObject& gameObject, const GameObjectType& gameObjectType)
	{
		std::stringstream name;

		if(gameObject.GetName().size() > 0)
		{
			name << mapName << "_" << gameObject.GetName();
		}
		else
		{
			//No name, generate one
			name << mapName << "_" << gameObjectType.GetName() << "_" << std::dec << (u32)gameObject.GetId();
		}

		return name.str();
	}
}

bool Project::ExportSceneAnimations(MapId mapId, const std::string& filename, ExportFormat format)
{
	Map& map = m_maps.find(mapId)->second;
	const std::string& mapName = map.GetName();

	ion::io::File file(filename, ion::io::File::eOpenWrite);
	if (file.IsOpen())
	{
		std::stringstream stream;
		WriteFileHeader(stream);

		for (TAnimationMap::const_iterator it = m_animations.begin(), end = m_animations.end(); it != end; ++it)
		{
			const Animation& animation = it->second;

			const int megaDriveFramesPerSecond = 60;
			const int keyframesPerSecond = 15;
			const int numKeyframes = (int)animation.GetLength() * keyframesPerSecond;
			float keyframeStep = animation.GetLength() / numKeyframes;
			float megaDriveFramesPerKeyframe = (keyframeStep * megaDriveFramesPerSecond);

			//STRUCT_BEGIN SceneAnimation
			//	SceneAnim_ActorList            rs.l 1
			//	SceneAnim_KeyframeTimesList    rs.l 1
			//	SceneAnim_KeyframeTrackListPos rs.l 1
			//	SceneAnim_ActorCount           rs.w 1
			//	SceneAnim_KeyframeCount        rs.w 1
			//	SceneAnim_Looping              rs.b 1
			//STRUCT_END

			//; Scene animation - l1a1_BossTest1
			//SceneAnim_l1a1_BossTest1 :
			//	dc.l SceneAnim_l1a1_BossTest11_ActorList
			//	dc.l SceneAnim_l1a1_BossTest11_KeyframeTimes
			//	dc.l SceneAnim_l1a1_BossTest11_KeyframeTrackList_Pos
			//	dc.w 5
			//	dc.w 8
			//	dc.b 1
			//	even

			stream << "SceneAnim_" << mapName << "_" << animation.GetName() << "_ActorCount\t\tequ " << animation.GetActorCount() << std::endl;
			stream << "SceneAnim_" << mapName << "_" << animation.GetName() << "_KeyframeCount\t\tequ " << numKeyframes << std::endl;
			stream << "SceneAnim_" << mapName << "_" << animation.GetName() << "_Looping\t\tequ " << ((animation.GetPlaybackBehaviour() == ion::render::Animation::eLoop) ? "1" : "0") << std::endl;

			stream << std::endl;

			stream << "; Scene animation - " << mapName << "_" << animation.GetName() << std::endl;
			stream << "SceneAnim_" << mapName << "_" << animation.GetName() << ":" << std::endl;
			stream << "\tdc.l SceneAnim_" << mapName << "_" << animation.GetName() << "_ActorList" << std::endl;
			stream << "\tdc.l SceneAnim_" << mapName << "_" << animation.GetName() << "_KeyframeTimes" << std::endl;
			stream << "\tdc.l SceneAnim_" << mapName << "_" << animation.GetName() << "_KeyframeTrackList_Pos" << std::endl;
			stream << "\tdc.w SceneAnim_" << mapName << "_" << animation.GetName() << "_ActorCount" << std::endl;
			stream << "\tdc.w SceneAnim_" << mapName << "_" << animation.GetName() << "_KeyframeCount" << std::endl;
			stream << "\tdc.b SceneAnim_" << mapName << "_" << animation.GetName() << "_Looping" << std::endl;
			stream << "\teven" << std::endl;

			stream << std::endl;

			//; Actor list
			//SceneAnim_l1a1_BossTest11_ActorList:
			//	dc.l EntityPoolStart_Core; Pool
			//	dc.l l1a1_Core_3_idx*Core_Struct_Size; Offset
			//	dc.l EntityPoolStart_Joint; Pool
			//	dc.l l1a1_Joint_4_idx*Joint_Struct_Size; Offset
			//	dc.l EntityPoolStart_Joint; Pool
			//	dc.l l1a1_Joint_5_idx*Joint_Struct_Size; Offset
			//	dc.l EntityPoolStart_Joint; Pool
			//	dc.l l1a1_Joint_6_idx*Joint_Struct_Size; Offset
			//	dc.l EntityPoolStart_Joint; Pool
			//	dc.l l1a1_Joint_7_idx*Joint_Struct_Size; Offset

			stream << "; Actor list" << std::endl;
			stream << "SceneAnim_" << mapName << "_" << animation.GetName() << "_ActorList:" << std::endl;

			for (TAnimActorMap::const_iterator actorIt = animation.ActorsBegin(), actorEnd = animation.ActorsEnd(); actorIt != actorEnd; ++actorIt)
			{
				const GameObject* gameObject = map.GetGameObject(actorIt->second.GetGameObjectId());
				const GameObjectType* gameObjectType = GetGameObjectType(gameObject->GetTypeId());
				std::string objectName = GenerateObjectName(mapName, *gameObject, *gameObjectType);

				stream << "\tdc.l EntityPoolStart_" << gameObjectType->GetName() << "\t; Pool" << std::endl;
				stream << "\tdc.l " << objectName << "_idx*" << gameObjectType->GetName() << "_Struct_Size\t; Offset" << std::endl;
			}

			stream << std::endl;

			//; Keyframe times
			//SceneAnim_l1a1_BossTest11_KeyframeTimes :
			//	dc.w 0x0000
			//	dc.w 0x0004
			//	dc.w 0x0008
			//	dc.w 0x000C
			//	dc.w 0x0010
			//	dc.w 0x0014
			//	dc.w 0x0018
			//	dc.w 0x001C

			stream << "; Keyframe times" << std::endl;
			stream << "SceneAnim_" << mapName << "_" << animation.GetName() << "_KeyframeTimes:" << std::endl;

			for (int i = 0; i < numKeyframes; i++)
			{
				stream << "\tdc.w 0x" << HEX4((int)(megaDriveFramesPerKeyframe * i)) << std::endl;
			}

			stream << std::endl;

			//; Keyframe tracks (position)
			//SceneAnim_l1a1_BossTest11_KeyframeTrackList_Pos:
			//	dc.l SceneAnim_l1a1_BossTest1_KeyframeTrack_Pos_l1a1_Core_3
			//	dc.l SceneAnim_l1a1_BossTest1_KeyframeTrack_Pos_l1a1_Joint_4
			//	dc.l SceneAnim_l1a1_BossTest1_KeyframeTrack_Pos_l1a1_Joint_5
			//	dc.l SceneAnim_l1a1_BossTest1_KeyframeTrack_Pos_l1a1_Joint_6
			//	dc.l SceneAnim_l1a1_BossTest1_KeyframeTrack_Pos_l1a1_Joint_7

			stream << "; Keyframe tracks (position)" << std::endl;
			stream << "SceneAnim_" << mapName << "_" << animation.GetName() << "_KeyframeTrackList_Pos:" << std::endl;

			for (TAnimActorMap::const_iterator actorIt = animation.ActorsBegin(), actorEnd = animation.ActorsEnd(); actorIt != actorEnd; ++actorIt)
			{
				const GameObject* gameObject = map.GetGameObject(actorIt->second.GetGameObjectId());
				const GameObjectType* gameObjectType = GetGameObjectType(gameObject->GetTypeId());
				std::string objectName = GenerateObjectName(mapName, *gameObject, *gameObjectType);

				stream << "\tdc.l SceneAnim_" << mapName << "_" << animation.GetName() << "_KeyframeTrack_Pos_" << objectName << std::endl;
			}

			stream << std::endl;

			for (TAnimActorMap::const_iterator actorIt = animation.ActorsBegin(), actorEnd = animation.ActorsEnd(); actorIt != actorEnd; ++actorIt)
			{
				//; Keyframe track(position, actor Core_3)
				//SceneAnim_l1a1_BossTest1_KeyframeTrack_Pos_l1a1_Core_3:
				//	dc.w 0x0001, 0x0000
				//	dc.w 0x0000, 0x0001
				//	dc.w 0x0002, 0x0000
				//	dc.w 0x0000, 0x0002
				//	dc.w 0x0003, 0x0000
				//	dc.w 0x0000, 0x0003
				//	dc.w 0x0004, 0x0000
				//	dc.w 0x0000, 0x0004

				const GameObject* gameObject = map.GetGameObject(actorIt->second.GetGameObjectId());
				const GameObjectType* gameObjectType = GetGameObjectType(gameObject->GetTypeId());
				std::string objectName = GenerateObjectName(mapName, *gameObject, *gameObjectType);

				stream << "; Keyframe track (position, actor " << objectName << ")" << std::endl;
				stream << "SceneAnim_" << mapName << "_" << animation.GetName() << "_KeyframeTrack_Pos_" << objectName << ":" << std::endl;

				ion::Vector2i lastPosition = actorIt->second.m_trackPosition.GetValue(0.0f);

				for (int i = 1; i < numKeyframes + 1; i++)
				{
					ion::Vector2i position = actorIt->second.m_trackPosition.GetValue(keyframeStep * i);
					ion::Vector2i delta = position - lastPosition;
					ion::Vector2 velocity((float)delta.x / megaDriveFramesPerKeyframe, (float)delta.y / megaDriveFramesPerKeyframe);
					lastPosition = position;
					stream << "\tdc.l 0x" << HEX8(ion::maths::FloatToFixed1616(velocity.x)) << ", 0x" << HEX8(ion::maths::FloatToFixed1616(velocity.y)) << std::endl;
				}

				stream << std::endl;
			}
		}

		stream << std::endl;
		file.Write(stream.str().c_str(), stream.str().size());
		file.Close();

		return true;
	}

	return false;
}

bool Project::ExportGameObjects(MapId mapId, const std::string& filename, ExportFormat format)
{
	Map& map = m_maps.find(mapId)->second;
	const std::string& mapName = map.GetName();

	ion::io::File file(filename, ion::io::File::eOpenWrite);
	if (file.IsOpen())
	{
		if (format == ExportFormat::Beehive)
		{
			ion::io::Archive archive(file, ion::io::Archive::Direction::Out);
			archive.SetContentType(ion::io::Archive::Content::Minimal);
			archive.Serialise(map.GetGameObjects(), "gameObjects");
			return true;
		}
		else
		{
			std::stringstream stream;
			WriteFileHeader(stream);

			const TGameObjectPosMap& gameObjMap = map.GetGameObjects();

			//Sort by init priority
			std::vector<const GameObjectType*> sortedTypes;
			for (TGameObjectTypeMap::const_iterator it = m_gameObjectTypes.begin(), end = m_gameObjectTypes.end(); it != end; ++it)
			{
				sortedTypes.push_back(&it->second);
			}

			std::sort(sortedTypes.begin(), sortedTypes.end(), [](const GameObjectType* elemA, const GameObjectType* elemB) { return elemA->GetInitPriority() < elemB->GetInitPriority(); });

			//Bake out all game object types, even if there's no game objects to go with them (still need the count and init subroutine)
			for (int i = 0; i < sortedTypes.size(); i++)
			{
				const GameObjectType& gameObjectType = *sortedTypes[i];
				const TGameObjectPosMap::const_iterator gameObjIt = gameObjMap.find(gameObjectType.GetId());
				u32 count = (gameObjIt != gameObjMap.end()) ? gameObjIt->second.size() : 0;

				//Export game object count
				stream << mapName << "_" << gameObjectType.GetName() << "_count equ 0x" << count << std::endl;

				//Export game object names/indices
				if (gameObjIt != gameObjMap.end())
				{
					for (int i = 0; i < gameObjIt->second.size(); i++)
					{
						std::string name = GenerateObjectName(mapName, gameObjIt->second[i].m_gameObject, gameObjectType);
						stream << name << "_idx\tequ 0x" << std::hex << i << std::endl;
					}

					stream << std::endl;

					stream << '\t' << "IFND FINAL" << std::endl;

					for (int i = 0; i < gameObjIt->second.size(); i++)
					{
						std::string name = GenerateObjectName(mapName, gameObjIt->second[i].m_gameObject, gameObjectType);

						stream << name << "_name\tdc.b \"";
						for (int i = 0; i < name.size(); i++)
						{
							stream << name[i];
						}

						stream << "\",0" << std::endl;
					}

					stream << "\teven" << std::endl;

					stream << '\t' << "ENDIF" << std::endl;
				}

				stream << std::endl;
			}

			stream << std::endl;

			//Output debug checks
#if defined OUTPUT_GAMEOBJ_DEBUG_CHECKS
			for (int i = 0; i < sortedTypes.size(); i++)
			{
				const GameObjectType& gameObjectType = *sortedTypes[i];
				const TGameObjectPosMap::const_iterator gameObjIt = gameObjMap.find(gameObjectType.GetId());
				u32 count = (gameObjIt != gameObjMap.end()) ? gameObjIt->second.size() : 0;

				stream << "\tIF " << mapName << "_" << gameObjectType.GetName() << "_count>" << gameObjectType.GetName() << "_MaxEntities" << std::endl;
				stream << "\tinform 2,\"Entity array overflow(" << gameObjectType.GetName() << ") - array size: 0x\%d count : 0x\%d\"," << gameObjectType.GetName() << "_MaxEntities," << mapName << "_" << gameObjectType.GetName() << "_count" << std::endl;
				stream << "\tENDIF" << std::endl;
				stream << std::endl;
			}
#endif

			stream << std::endl;

			//Start init subroutine
			stream << mapName << "_LoadGameObjects:" << std::endl;

			for (int i = 0; i < sortedTypes.size(); i++)
			{
				const GameObjectType& gameObjectType = *sortedTypes[i];
				const TGameObjectPosMap::const_iterator gameObjIt = gameObjMap.find(gameObjectType.GetId());

				//Export all game objects of this type
				if (gameObjIt != gameObjMap.end())
				{
					if (gameObjIt->second.size() > 0)
					{
						//Alloc memory from entity pool
						stream << '\t' << "PUSHL  a1" << std::endl;
						stream << '\t' << "RAMPOOL_ALLOC Pool_Entities, #(" << gameObjectType.GetName() << "_Struct_Size*" << mapName << "_" << gameObjectType.GetName() << "_count)" << std::endl;
						stream << '\t' << "move.l a1, EntityPoolStart_" << gameObjectType.GetName() << std::endl;
						stream << '\t' << "move.l a1, a0" << std::endl;
						stream << '\t' << "POPL   a1" << std::endl;
						stream << std::endl;

						for (int i = 0; i < gameObjIt->second.size(); i++)
						{
							std::string name = GenerateObjectName(mapName, gameObjIt->second[i].m_gameObject, gameObjectType);
							gameObjIt->second[i].m_gameObject.Export(stream, gameObjectType, name, map.GetWidth() * m_platformConfig.tileWidth);
							stream << '\t' << "add.l #" << gameObjectType.GetName() << "_Struct_Size, a0" << std::endl;
							stream << std::endl;
						}
					}
				}

				stream << std::endl;
			}

			//End subroutine
			stream << '\t' << "rts" << std::endl;
			stream << std::endl;

			file.Write(stream.str().c_str(), stream.str().size());

			return true;
		}
	}

	return false;
}

bool Project::ExportSpriteSheets(const std::string& directory, ExportFormat format)
{
	for(TActorMap::iterator it = m_actors.begin(), end = m_actors.end(); it != end; ++it)
	{
		//if(binary)
		//{
		//	std::string binaryFilename = filename.substr(0, filename.find_first_of('.'));
		//	binaryFilename += ".bin";
		//
		//	//Export binary data
		//	ion::io::File binaryFile(binaryFilename, ion::io::File::eOpenWrite);
		//	if(binaryFile.IsOpen())
		//	{
		//		collisionMap.Export(*this, binaryFile);
		//		binarySize = binaryFile.GetSize();
		//	}
		//	else
		//	{
		//		return false;
		//	}
		//}

		if (format == ExportFormat::Beehive)
		{
			std::stringstream filename;
			filename << directory << "\\" << it->second.GetName() << ".bee";

			ion::io::File file(filename.str(), ion::io::File::eOpenWrite);
			if (file.IsOpen())
			{
				ion::io::Archive archive(file, ion::io::Archive::Direction::Out);
				archive.SetContentType(ion::io::Archive::Content::Minimal);
				archive.Serialise(it->second, "actor");
			}
			else
			{
				return false;
			}
		}
		else
		{
			std::stringstream filename;
			filename << directory << "\\" << it->second.GetName() << ".ASM";

			ion::io::File file(filename.str(), ion::io::File::eOpenWrite);
			if(file.IsOpen())
			{
				std::stringstream stream;
				WriteFileHeader(stream);
				it->second.ExportSpriteSheets(m_platformConfig, stream);
				file.Write(stream.str().c_str(), stream.str().size());
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

bool Project::ExportSpriteAnims(const std::string& directory, ExportFormat format) const
{
	for(TActorMap::const_iterator it = m_actors.begin(), end = m_actors.end(); it != end; ++it)
	{
		//if(binary)
		//{
		//
		//}
		//else
		{
			std::stringstream filename;
			filename << directory << "\\" << it->second.GetName() << ".ASM";

			ion::io::File file(filename.str(), ion::io::File::eOpenWrite);
			if(file.IsOpen())
			{
				std::stringstream stream;
				WriteFileHeader(stream);
				it->second.ExportSpriteAnims(m_platformConfig, stream);
				file.Write(stream.str().c_str(), stream.str().size());
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}

bool Project::ExportStampAnims(const std::string& filename, ExportFormat format) const
{
	ion::io::File file(filename, ion::io::File::eOpenWrite);
	if(file.IsOpen())
	{
		//if(binary)
		//{
		//
		//}
		//else
		{
			std::stringstream stream;
			WriteFileHeader(stream);

			//Export setup code snippet
			stream << m_name << "_LoadSceneAnimations:" << std::endl;
			stream << "\tPUSHL  a1" << std::endl;
			stream << "\tlea    StampAnimObjs, a0" << std::endl;
			stream << "\tlea    StampAnims, a1" << std::endl;
			stream << std::endl;

			for(TStampMap::const_iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
			{
				it->second.ExportStampAnimSetup(m_platformConfig, stream);
			}

			stream << "\tPOPL   a1" << std::endl;
			stream << "\trts" << std::endl;
			stream << std::endl;

			//Export anims
			for(TStampMap::const_iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
			{
				it->second.ExportStampAnims(m_platformConfig, stream);
			}

			file.Write(stream.str().c_str(), stream.str().size());
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool Project::ExportSpritePalettes(const std::string& directory) const
{
	for(TActorMap::const_iterator it = m_actors.begin(), end = m_actors.end(); it != end; ++it)
	{
		std::stringstream filename;
		filename << directory << "\\" << it->second.GetName() << ".ASM";

		ion::io::File file(filename.str(), ion::io::File::eOpenWrite);
		if(file.IsOpen())
		{
			std::stringstream stream;
			WriteFileHeader(stream);
			it->second.ExportSpritePalettes(m_platformConfig, stream);
			file.Write(stream.str().c_str(), stream.str().size());
		}
		else
		{
			return false;
		}
	}

	return true;
}

bool Project::ExportMapBitmaps(const std::string& directory) const
{
	for(TMapMap::const_iterator it = m_maps.begin(), end = m_maps.end(); it != end; ++it)
	{
		std::stringstream filename;
		filename << directory << "\\map_" << it->first << ".bmp";

		const Map& map = it->second;

		BMPReader writer;
		writer.SetDimensions(map.GetWidth() * m_platformConfig.tileWidth, map.GetHeight() * m_platformConfig.tileHeight);

		const Palette* palette = GetPalette(0);

		if(palette)
		{
			for(int i = 0; i < Palette::coloursPerPalette && palette->IsColourUsed(i); i++)
			{
				Colour colour = palette->GetColour(i);
				writer.SetPaletteEntry(i, BMPReader::Colour(colour.GetRed(), colour.GetGreen(), colour.GetBlue()));
			}

			for(int tileX = 0; tileX < map.GetWidth(); tileX++)
			{
				for(int tileY = 0; tileY < map.GetHeight(); tileY++)
				{
					//Find stamp at position first
					ion::Vector2i stampPos;
					TileId tileId = InvalidTileId;
					u32 flags = 0;
					u32 mapEntryIndex = 0;
					StampId stampId = map.FindStamp(tileX, tileY, stampPos, flags, mapEntryIndex);

					if(stampId)
					{
						//Get from stamp
						if(const Stamp* stamp = GetStamp(stampId))
						{
							ion::Vector2i offset = ion::Vector2i(tileX, tileY) - stampPos;

							int sourceX = (flags & Map::eFlipX) ? (stamp->GetWidth() - 1 - offset.x) : offset.x;
							int sourceY = (flags & Map::eFlipY) ? (stamp->GetHeight() - 1 - offset.y) : offset.y;

							tileId = stamp->GetTile(sourceX, sourceY);
							flags ^= stamp->GetTileFlags(sourceX, sourceY);
						}
					}
					else
					{
						//Pick tile
						tileId = map.GetTile(tileX, tileY);
						flags = map.GetTileFlags(tileX, tileY);
					}

					if(tileId == InvalidTileId)
					{
						tileId = m_backgroundTile;

						if(tileId == InvalidTileId)
						{
							tileId = 0;
						}
					}

					const Tile* tile = m_tileset.GetTile(tileId);

					for(int x = 0; x < m_platformConfig.tileWidth; x++)
					{
						for(int y = 0; y < m_platformConfig.tileHeight; y++)
						{
							u8 colourIndex = tile->GetPixelColour(x, y);
							int pixelX = (flags & Map::eFlipX) ? (m_platformConfig.tileWidth - x - 1) : x;
							int pixelY = (flags & Map::eFlipY) ? (m_platformConfig.tileHeight - y - 1) : y;
							writer.SetColourIndex((tileX * m_platformConfig.tileWidth) + pixelX, (tileY * m_platformConfig.tileHeight) + pixelY, colourIndex);
						}
					}
				}
			}
		}

		writer.Write(filename.str());
	}

	return true;
}

bool Project::ExportStampBitmaps(const std::string& directory) const
{
	for(TStampMap::const_iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
	{
		std::stringstream filename;
		filename << directory << "\\stamp_" << it->first << ".bmp";

		const Stamp& stamp = it->second;

		BMPReader writer;
		writer.SetDimensions(stamp.GetWidth() * m_platformConfig.tileWidth, stamp.GetHeight() * m_platformConfig.tileHeight);

		const Tile* firstTile = m_tileset.GetTile(stamp.GetTile(0, 0));
		if(!firstTile)
		{
			firstTile = m_tileset.GetTile(GetBackgroundTile());
		}

		if(!firstTile)
		{
			firstTile = m_tileset.GetTile(0);
		}

		const Palette& palette = *GetPalette(firstTile->GetPaletteId());
		for(int i = 0; i < Palette::coloursPerPalette && palette.IsColourUsed(i); i++)
		{
			Colour colour = palette.GetColour(i);
			writer.SetPaletteEntry(i, BMPReader::Colour(colour.GetRed(), colour.GetGreen(), colour.GetBlue()));
		}
		
		for(int tileX = 0; tileX < stamp.GetWidth(); tileX++)
		{
			for(int tileY = 0; tileY < stamp.GetHeight(); tileY++)
			{
				TileId tileId = stamp.GetTile(tileX, tileY);
				u32 tileFlags = stamp.GetTileFlags(tileX, tileY);
				const Tile* tile = m_tileset.GetTile(tileId);

				if(!tile)
				{
					tile = m_tileset.GetTile(GetBackgroundTile());
				}

				if(!tile)
				{
					tile = m_tileset.GetTile(0);
				}

				if(tile)
				{
					for(int x = 0; x < m_platformConfig.tileWidth; x++)
					{
						for(int y = 0; y < m_platformConfig.tileHeight; y++)
						{
							u8 colourIndex = tile->GetPixelColour(x, y);
							int pixelX = (tileFlags & Map::eFlipX) ? (m_platformConfig.tileWidth - x - 1) : x;
							int pixelY = (tileFlags & Map::eFlipY) ? (m_platformConfig.tileHeight - y - 1) : y;
							writer.SetColourIndex((tileX * m_platformConfig.tileWidth) + pixelX, (tileY * m_platformConfig.tileHeight) + pixelY, colourIndex);
						}
					}
				}
			}
		}

		writer.Write(filename.str());
	}

	return true;
}
