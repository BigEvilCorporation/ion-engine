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

#include "PlatformConfig.h"
#include "Tile.h"
#include "TerrainTile.h"
#include "Stamp.h"
#include "GameObject.h"

class Project;

struct StampMapEntry
{
	StampMapEntry() { m_id = 0; m_flags = 0; }
	StampMapEntry(StampId stampId, u32 flags, const ion::Vector2i& position, const ion::Vector2i& size)
	{
		m_id = stampId;
		m_flags = flags;
		m_position = position;
		m_size = size;
	}

	void Serialise(ion::io::Archive& archive)
	{
		archive.Serialise(m_id);
		archive.Serialise(m_flags);
		archive.Serialise(m_position);
		archive.Serialise(m_size);
	}

	StampId m_id;
	u32 m_flags;
	ion::Vector2i m_position;
	ion::Vector2i m_size;
};

struct GameObjectMapEntry
{
	GameObjectMapEntry() { }
	GameObjectMapEntry(const GameObject& gameObject, const ion::Vector2i& position, const ion::Vector2i& size)
	{
		m_gameObject = gameObject;
		//m_position = position;
		m_size = size;
	}

	void Serialise(ion::io::Archive& archive)
	{
		archive.Serialise(m_gameObject);
		//archive.Serialise(m_position);
		archive.Serialise(m_size);
	}

	GameObject m_gameObject;
	//ion::Vector2i m_position;
	ion::Vector2i m_size;
};

typedef std::vector<StampMapEntry> TStampPosMap;
typedef std::map< GameObjectTypeId, std::vector<GameObjectMapEntry> > TGameObjectPosMap;

class Map
{
public:
	enum TileFlags
	{
		eFlipX = 1<<0,
		eFlipY = 1<<1,
		eHighPlane = 1<<2
	};

	struct TileDesc
	{
		TileDesc() { m_id = InvalidTileId; m_flags = 0; }
		TileDesc(TileId id, u32 flags) { m_id = id; m_flags = flags; }

		void Serialise(ion::io::Archive& archive)
		{
			archive.Serialise(m_id);
			archive.Serialise(m_flags);
		}

		bool operator ==(const TileDesc& rhs) const
		{
			return m_id == rhs.m_id && m_flags == rhs.m_flags;
		}

		TileId m_id;
		u32 m_flags;
	};

	struct Block
	{
		std::vector<TileDesc> m_tiles;
		int uniqueIndex = -1;
		bool unique = false;

		bool operator ==(const Block& rhs) const;
		void Export(const Project& project, std::stringstream& stream, int blockWidth, int blockHeight);
		void Export(const Project& project, ion::io::File& file, int blockWidth, int blockHeight);
	};

	Map();
	Map(const PlatformConfig& platformConfig);

	void SetPlatformConfig(const PlatformConfig& platformConfig) { m_platformConfig = &platformConfig; }

	void SetName(const std::string& name) { m_name = name; }
	const std::string& GetName() const { return m_name; }

	void Clear();

	int GetWidth() const;
	int GetHeight() const;

	int GetBlockAlignedWidth(int blockWidth) const;
	int GetBlockAlignedHeight(int blockHeight) const;

	int GetWidthBlocks(int blockWidth) const;
	int GetHeightBlocks(int blockHeight) const;

	void Resize(int w, int h, bool shiftRight, bool shiftDown);

	//Set tile on map
	void SetTile(int x, int y, TileId tile);
	TileId GetTile(int x, int y) const;

	//Set tile flags
	void SetTileFlags(int x, int y, u32 flags);
	u32 GetTileFlags(int x, int y) const;

	//Fill tiles on map
	void FillTiles(TileId tileId, const std::vector<ion::Vector2i>& selection);

	//Set stamp on map
	void SetStamp(int x, int y, const Stamp& stamp, u32 flipFlags);
	void BakeStamp(int x, int y, const Stamp& stamp, u32 flipFlags);
	StampId FindStamp(int x, int y, ion::Vector2i& topLeft, u32& flags, u32& mapEntryIndex) const;
	int FindStamps(int x, int y, int width, int height, std::vector<const StampMapEntry*>& stamps) const;
	void MoveStamp(StampId stampId, u32 mapEntryIndex, int x, int y, int& originalX, int& originalY);
	void RemoveStamp(StampId stampId, int x, int y);
	void StampBringToFront(int x, int y, StampId stampId);
	void StampSendToBack(int x, int y, StampId stampId);
	TStampPosMap& GetStamps();
	const TStampPosMap& GetStamps() const;

	//Place game object on map
	GameObjectId PlaceGameObject(int x, int y, const GameObjectType& objectType);
	GameObjectId PlaceGameObject(int x, int y, int width, int height, const GameObjectType& objectType);
	GameObjectId PlaceGameObject(int x, int y, const GameObject& object, const GameObjectType& objectType);
	GameObjectId FindGameObject(int x, int y, ion::Vector2i& topLeft) const;
	GameObject* FindGameObject(const std::string& name);
	int FindGameObjects(int x, int y, int width, int height, std::vector<const GameObjectMapEntry*>& gameObjects) const;
	GameObject* GetGameObject(GameObjectId gameObjectId);
	void MoveGameObject(GameObjectId gameObjectId, int x, int y);
	void RemoveGameObject(int x, int y);
	const TGameObjectPosMap& GetGameObjects() const;
	TGameObjectPosMap& GetGameObjects();

	const TStampPosMap::const_iterator StampsBegin() const;
	const TStampPosMap::const_iterator StampsEnd() const;
	TStampPosMap::iterator StampsBegin();
	TStampPosMap::iterator StampsEnd();

	// Generate NxN blocks and sort unique
	void GenerateBlocks(const Project& project, int blockWidth, int blockHeight);
	std::vector<Block>& GetBlocks();
	const std::vector<Block>& GetBlocks() const;

	void Serialise(ion::io::Archive& archive);
	void Export(const Project& project, std::vector<TileDesc>& tileMap) const;
	void Export(const Project& project, std::stringstream& stream) const;
	void Export(const Project& project, ion::io::File& file) const;
	void ExportBlockMap(const Project& project, std::stringstream& stream, int blockWidth, int blockHeight) const;
	void ExportBlockMap(const Project& project, ion::io::File& file, int blockWidth, int blockHeight, std::vector<u32>& colOffsets) const;
	void ExportStampMap(const Project& project, std::stringstream& stream) const;
	void ExportStampMap(const Project& project, ion::io::File& file) const;

	//Last used export filenames
	struct ExportFilenames
	{
		std::string map;
		std::string stampMap;
		std::string collisionMap;
		std::string sceneAnims;
		std::string gameObjects;
		std::string blocks;
		std::string blockMap;
		std::string terrainBlocks;
		std::string terrainBlockMap;

		bool mapExportEnabled = false;
		bool stampMapExportEnabled = false;
		bool collisionMapExportEnabled = false;
		bool sceneAnimExportEnabled = false;
		bool gameObjectsExportEnabled = false;
		bool blocksExportEnabled = false;
		bool blockMapExportEnabled = false;
		bool terrainBlocksExportEnabled = false;
		bool terrainBlockMapExportEnabled = false;

		void Serialise(ion::io::Archive& archive)
		{
			archive.Serialise(mapExportEnabled, "mapExportEnabled");
			archive.Serialise(stampMapExportEnabled, "stampMapExportEnabled");
			archive.Serialise(blockMapExportEnabled, "blockMapExportEnabled");
			archive.Serialise(terrainBlocksExportEnabled, "terrainBlocksExportEnabled");
			archive.Serialise(terrainBlockMapExportEnabled, "terrainBlockMapExportEnabled");
			archive.Serialise(collisionMapExportEnabled, "collisionMapExportEnabled");
			archive.Serialise(sceneAnimExportEnabled, "sceneAnimExportEnabled");
			archive.Serialise(gameObjectsExportEnabled, "gameObjectsExportEnabled");

			archive.Serialise(map, "exportFNameMap");
			archive.Serialise(stampMap, "exportFNameStampMap");
			archive.Serialise(blockMap, "exportFNameblockMap");
			archive.Serialise(terrainBlocks, "exportFNameTerrainBlocks");
			archive.Serialise(terrainBlockMap, "exportFNameTerrainBlockMap");
			archive.Serialise(collisionMap, "exportFNameCollisionMap");
			archive.Serialise(sceneAnims, "exportFNameSceneAnims");
			archive.Serialise(gameObjects, "exportFNameGameObjects");
		}
	};

	ExportFilenames m_exportFilenames;

private:

	void BakeStamp(std::vector<TileDesc>& tiles, int mapWidth, int mapHeight, int x, int y, const Stamp& stamp, u32 flipFlags) const;

	const PlatformConfig* m_platformConfig;
	std::string m_name;
	int m_width;
	int m_height;
	std::vector<TileDesc> m_tiles;
	TStampPosMap m_stamps;
	TGameObjectPosMap m_gameObjects;
	GameObjectId m_nextFreeGameObjectId;

	std::vector<Block> m_blocks;
};