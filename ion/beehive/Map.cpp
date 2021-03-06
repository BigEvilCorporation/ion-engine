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

#include "Map.h"
#include "Project.h"
#include <algorithm>

#include <ion/core/string/String.h>
#include <ion/core/memory/Memory.h>
#include <ion/core/memory/Endian.h>
#include <ion/maths/Geometry.h>

#include <ion/dependencies/slz/tool/main.h>
#include <ion/dependencies/slz/tool/compress.h>
#include <ion/dependencies/slz/tool/decompress.h>
#include <ion/io/compression/CompressionRLE.h>

#define HEX1(val) std::hex << std::setfill('0') << std::setw(1) << std::uppercase << (int)val
#define HEX2(val) std::hex << std::setfill('0') << std::setw(2) << std::uppercase << (int)val
#define HEX4(val) std::hex << std::setfill('0') << std::setw(4) << std::uppercase << (int)val
#define HEX8(val) std::hex << std::setfill('0') << std::setw(8) << std::uppercase << (int)val

Map::Map()
{
	m_platformConfig = &PlatformPresets::s_configs[PlatformPresets::ePresetMegaDrive];
	m_name = "Unnamed";
	m_width = 0;
	m_height = 0;
	m_nextFreeGameObjectId = 1;
}

Map::Map(const PlatformConfig& platformConfig)
{
	m_platformConfig = &platformConfig;
	m_name = "Unnamed";
	m_width = 0;
	m_height = 0;
	m_nextFreeGameObjectId = 1;
	Resize(platformConfig.scrollPlaneWidthTiles, platformConfig.scrollPlaneHeightTiles, false, false);
}

void Map::Clear()
{
	//Clear tiles
	for(int x = 0; x < m_width; x++)
	{
		for(int y = 0; y < m_height; y++)
		{
			SetTile(x, y, InvalidTileId);
		}
	}

	//Clear stamps
	m_stamps.clear();
}

void Map::Serialise(ion::io::Archive& archive)
{
	archive.Serialise(m_name, "name");
	archive.Serialise(m_width, "width");
	archive.Serialise(m_height, "height");
	archive.Serialise(m_tiles, "tiles");
	archive.Serialise(m_stamps, "stamps");
	archive.Serialise(m_gameObjects, "gameObjects");
	archive.Serialise(m_exportFilenames, "exportFilenames");
	archive.Serialise(m_nextFreeGameObjectId, "nextFreeGameObjId");
}

int Map::GetWidth() const
{
	return m_width;
}

int Map::GetHeight() const
{
	return m_height;
}

int Map::GetBlockAlignedWidth(int blockWidth) const
{
	return ion::maths::RoundUpToNearest((int)m_width, blockWidth);
}

int Map::GetBlockAlignedHeight(int blockHeight) const
{
	return ion::maths::RoundUpToNearest((int)m_height, blockHeight);
}

int Map::GetWidthBlocks(int blockWidth) const
{
	return GetBlockAlignedWidth(blockWidth) / blockWidth;
}

int Map::GetHeightBlocks(int blockHeight) const
{
	return GetBlockAlignedHeight(blockHeight) / blockHeight;
}

void Map::Resize(int width, int height, bool shiftRight, bool shiftDown)
{
	const int tileWidth = m_platformConfig->tileWidth;
	const int tileHeight = m_platformConfig->tileHeight;

	//Create new tile array
	std::vector<TileDesc> tiles;
	std::vector<TerrainTileId> TerrainTiles;

	//Set new size
	int size = width * height;
	tiles.resize(size);
	TerrainTiles.resize(size);

	//Fill with invalid tile
	TileDesc blankTile;
	blankTile.m_id = InvalidTileId;
	std::fill(tiles.begin(), tiles.end(), blankTile);

	//Copy tiles
	for(int x = 0; x < m_width; x++)
	{
		for(int y = 0; y < m_height; y++)
		{
			int destTileIdx = (y * width) + x;
			if(shiftRight && width > m_width)
				destTileIdx += (width - m_width);
			if(shiftRight && width < m_width)
				destTileIdx -= (m_width - width);
			if(shiftDown && height > m_height)
				destTileIdx += (height - m_height) * width;
			if(shiftDown && height < m_height)
				destTileIdx -= (m_height - height) * width;

			if(destTileIdx >= 0 && destTileIdx < size)
			{
				tiles[destTileIdx].m_id = GetTile(x, y);
				tiles[destTileIdx].m_flags = GetTileFlags(x, y);
			}
		}
	}

	if(shiftRight && width > m_width)
	{
		//Shift stamps
		for(TStampPosMap::iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
		{
			it->m_position.x += (width - m_width);
		}

		//Shift game objects
		for(TGameObjectPosMap::iterator it = m_gameObjects.begin(), end = m_gameObjects.end(); it != end; ++it)
		{
			for(int i = 0; i < it->second.size(); i++)
			{
				//it->second[i].m_position.x += (width - m_width);
				ion::Vector2i pos = it->second[i].m_gameObject.GetPosition();
				pos.x += ((width - m_width) * tileWidth);
				it->second[i].m_gameObject.SetPosition(pos);
			}
		}
	}

	if(shiftRight && width < m_width)
	{
		//Shift stamps
		for(TStampPosMap::iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
		{
			it->m_position.x -= (m_width - width);
		}

		//Shift game objects
		for(TGameObjectPosMap::iterator it = m_gameObjects.begin(), end = m_gameObjects.end(); it != end; ++it)
		{
			for(int i = 0; i < it->second.size(); i++)
			{
				//it->second[i].m_position.x += (width - m_width);
				ion::Vector2i pos = it->second[i].m_gameObject.GetPosition();
				pos.x -= ((m_width - width) * tileWidth);
				it->second[i].m_gameObject.SetPosition(pos);
			}
		}
	}

	if(shiftDown && height > m_height)
	{
		//Shift stamps
		for(TStampPosMap::iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
		{
			it->m_position.y += (height - m_height);
		}

		//Shift game objects
		for(TGameObjectPosMap::iterator it = m_gameObjects.begin(), end = m_gameObjects.end(); it != end; ++it)
		{
			for(int i = 0; i < it->second.size(); i++)
			{
				//it->second[i].m_position.y += (height - m_height);
				ion::Vector2i pos = it->second[i].m_gameObject.GetPosition();
				pos.y += ((height - m_height) * tileWidth);
				it->second[i].m_gameObject.SetPosition(pos);
			}
		}
	}

	if(shiftDown && height < m_height)
	{
		//Shift stamps
		for(TStampPosMap::iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
		{
			it->m_position.y -= (m_height - height);
		}

		//Shift game objects
		for(TGameObjectPosMap::iterator it = m_gameObjects.begin(), end = m_gameObjects.end(); it != end; ++it)
		{
			for(int i = 0; i < it->second.size(); i++)
			{
				//it->second[i].m_position.y += (height - m_height);
				ion::Vector2i pos = it->second[i].m_gameObject.GetPosition();
				pos.y -= ((m_height - height) * tileWidth);
				it->second[i].m_gameObject.SetPosition(pos);
			}
		}
	}
	
	//Set new
	m_tiles = tiles;
	m_width = width;
	m_height = height;
}

void Map::SetTile(int x, int y, TileId tile)
{
	int tileIdx = (y * m_width) + x;
	ion::debug::Assert(tileIdx < (m_width * m_height), "Map::SetTile() - Out of range");
	m_tiles[tileIdx].m_id = tile;
	m_tiles[tileIdx].m_flags = 0;
}

TileId Map::GetTile(int x, int y) const
{
	int tileIdx = (y * m_width) + x;
	ion::debug::Assert(tileIdx < (m_width * m_height), "Map::GetTile() - Out of range");
	return m_tiles[tileIdx].m_id;
}

void Map::SetTileFlags(int x, int y, u32 flags)
{
	int tileIdx = (y * m_width) + x;
	ion::debug::Assert(tileIdx < (m_width * m_height), "Map::SetTileFlags() - Out of range");
	m_tiles[tileIdx].m_flags = flags;
}

u32 Map::GetTileFlags(int x, int y) const
{
	int tileIdx = (y * m_width) + x;
	ion::debug::Assert(tileIdx < (m_width * m_height), "Map::GetTileFlags() - Out of range");
	return m_tiles[tileIdx].m_flags;
}

void Map::FillTiles(TileId tileId, const std::vector<ion::Vector2i>& selection)
{
	for(int i = 0; i < selection.size(); i++)
	{
		SetTile(selection[i].x, selection[i].y, tileId);
	}
}

void Map::SetStamp(int x, int y, const Stamp& stamp, u32 flipFlags)
{
	//Add to stamp map
	m_stamps.push_back(StampMapEntry(stamp.GetId(), flipFlags, ion::Vector2i(x, y), ion::Vector2i(stamp.GetWidth(), stamp.GetHeight())));
}

void Map::BakeStamp(int x, int y, const Stamp& stamp, u32 flipFlags)
{
	BakeStamp(m_tiles, m_width, m_height, x, y, stamp, flipFlags);
}

void Map::BakeStamp(std::vector<TileDesc>& tiles, int mapWidth, int mapHeight, int x, int y, const Stamp& stamp, u32 flipFlags) const
{
	int width = stamp.GetWidth();
	int height = stamp.GetHeight();

	for(int stampX = 0; stampX < width; stampX++)
	{
		for(int stampY = 0; stampY < height; stampY++)
		{
			int sourceX = (flipFlags & eFlipX) ? (width - 1 - stampX) : stampX;
			int sourceY = (flipFlags & eFlipY) ? (height - 1 - stampY) : stampY;

			TileId tileId = stamp.GetTile(sourceX, sourceY);
			u32 tileFlags = stamp.GetTileFlags(sourceX, sourceY);
			tileFlags ^= flipFlags;

			int mapX = stampX + x;
			int mapY = stampY + y;

			//Can place stamps outside map boundaries, only bake tiles that are inside
			if(mapX >= 0 && mapX < mapWidth && mapY >= 0 && mapY < mapHeight)
			{
				//Copy tile and flags
				int tileIdx = (mapY * mapWidth) + mapX;
				tiles[tileIdx].m_id = tileId;
				tiles[tileIdx].m_flags = tileFlags;
			}
		}
	}
}

StampId Map::FindStamp(int x, int y, ion::Vector2i& topLeft, u32& flags, u32& mapEntryIndex) const
{
	StampId stampId = InvalidStampId;
	ion::Vector2i size;
	ion::Vector2i bottomRight;

	//Work backwards, find last placed stamp first
	int index = m_stamps.size() - 1;
	for(TStampPosMap::const_reverse_iterator it = m_stamps.rbegin(), end = m_stamps.rend(); it != end && !stampId; ++it)
	{
		topLeft = it->m_position;
		size = it->m_size;

		bottomRight = topLeft + size;

		if(x >= topLeft.x && y >= topLeft.y
			&& x < bottomRight.x && y < bottomRight.y)
		{
			stampId = it->m_id;
			flags = it->m_flags;
			mapEntryIndex = index;
		}

		index--;
	}

	return stampId;
}

int Map::FindStamps(int x, int y, int width, int height, std::vector<const StampMapEntry*>& stamps) const
{
	ion::Vector2i boundsMin(x, y);
	ion::Vector2i boundsMax(x + width, y + height);

	for(int i = 0; i < m_stamps.size(); i++)
	{
		const StampMapEntry& stamp = m_stamps[i];
		if(ion::maths::BoxIntersectsBox(boundsMin, boundsMax, stamp.m_position, stamp.m_position + stamp.m_size))
		{
			stamps.push_back(&stamp);
		}
	}

	return stamps.size();
}

void Map::MoveStamp(StampId stampId, u32 mapEntryIndex, int x, int y, int& originalX, int& originalY)
{
	ion::debug::Assert(mapEntryIndex < m_stamps.size(), "Map::MoveStamp() - Out of bounds");
	ion::debug::Assert(m_stamps[mapEntryIndex].m_id == stampId, "Map::MoveStamp() - StampId/map entry mismatch");

	originalX = m_stamps[mapEntryIndex].m_position.x;
	originalY = m_stamps[mapEntryIndex].m_position.y;
	m_stamps[mapEntryIndex].m_position.x = x;
	m_stamps[mapEntryIndex].m_position.y = y;
}

void Map::RemoveStamp(StampId stampId, int x, int y)
{
	ion::Vector2i size;
	ion::Vector2i bottomRight;

	for(TStampPosMap::reverse_iterator it = m_stamps.rbegin(), end = m_stamps.rend(); it != end; ++it)
	{
		ion::Vector2i topLeft = it->m_position;
		ion::Vector2i size = it->m_size;

		bottomRight = topLeft + size;

		if(it->m_id == stampId && x >= topLeft.x && y >= topLeft.y && x < bottomRight.x && y < bottomRight.y)
		{
			m_stamps.erase(std::next(it).base());
			
			return;
		}
	}
}

void Map::StampBringToFront(int x, int y, StampId stampId)
{
	for(int i = 0; i < m_stamps.size(); i++)
	{
		if(m_stamps[i].m_id == stampId && m_stamps[i].m_position.x == x && m_stamps[i].m_position.y == y)
		{
			StampMapEntry stamp = m_stamps[i];
			m_stamps.erase(m_stamps.begin() + i);
			m_stamps.push_back(stamp);
			return;
		}
	}
}

void Map::StampSendToBack(int x, int y, StampId stampId)
{
	for(int i = 0; i < m_stamps.size(); i++)
	{
		if(m_stamps[i].m_id == stampId && m_stamps[i].m_position.x == x && m_stamps[i].m_position.y == y)
		{
			StampMapEntry stamp = m_stamps[i];
			m_stamps.erase(m_stamps.begin() + i);
			m_stamps.insert(m_stamps.begin(), stamp);
			return;
		}
	}
}

TStampPosMap& Map::GetStamps()
{
	return m_stamps;
}

const TStampPosMap& Map::GetStamps() const
{
	return m_stamps;
}

GameObjectId Map::PlaceGameObject(int x, int y, const GameObjectType& objectType)
{
	TGameObjectPosMap::iterator it = m_gameObjects.find(objectType.GetId());
	if(it == m_gameObjects.end())
		it = m_gameObjects.insert(std::make_pair(objectType.GetId(), std::vector<GameObjectMapEntry>())).first;

	const int tileWidth = m_platformConfig->tileWidth;
	const int tileHeight = m_platformConfig->tileHeight;

	GameObjectId objectId = m_nextFreeGameObjectId++;
	GameObject gameObject(objectId, objectType.GetId(), ion::Vector2i(x * tileWidth, y * tileHeight));
	it->second.push_back(GameObjectMapEntry(gameObject, ion::Vector2i(x, y), ion::Vector2i(objectType.GetDimensions().x, objectType.GetDimensions().y)));
	return objectId;
}

GameObjectId Map::PlaceGameObject(int x, int y, int width, int height, const GameObjectType& objectType)
{
	TGameObjectPosMap::iterator it = m_gameObjects.find(objectType.GetId());
	if(it == m_gameObjects.end())
		it = m_gameObjects.insert(std::make_pair(objectType.GetId(), std::vector<GameObjectMapEntry>())).first;

	const int tileWidth = m_platformConfig->tileWidth;
	const int tileHeight = m_platformConfig->tileHeight;

	GameObjectId objectId = m_nextFreeGameObjectId++;
	GameObject gameObject(objectId, objectType.GetId(), ion::Vector2i(x * tileWidth, y * tileHeight), ion::Vector2i(width * tileWidth, height * tileHeight));
	it->second.push_back(GameObjectMapEntry(gameObject, ion::Vector2i(x, y), ion::Vector2i(objectType.GetDimensions().x, objectType.GetDimensions().y)));
	return objectId;
}

GameObjectId Map::PlaceGameObject(int x, int y, const GameObject& object, const GameObjectType& objectType)
{
	TGameObjectPosMap::iterator it = m_gameObjects.find(object.GetTypeId());
	if(it == m_gameObjects.end())
		it = m_gameObjects.insert(std::make_pair(object.GetTypeId(), std::vector<GameObjectMapEntry>())).first;

	//const int tileWidth = m_platformConfig->tileWidth;
	//const int tileHeight = m_platformConfig->tileHeight;

	GameObjectId objectId = m_nextFreeGameObjectId++;
	it->second.push_back(GameObjectMapEntry(GameObject(objectId, object), ion::Vector2i(x, y), ion::Vector2i(objectType.GetDimensions().x, objectType.GetDimensions().y)));
	return objectId;
}

GameObjectId Map::FindGameObject(int x, int y, ion::Vector2i& topLeft) const
{
	GameObjectId gameObjectId = InvalidGameObjectId;
	ion::Vector2i size;
	ion::Vector2i bottomRight;

	const int tileWidth = m_platformConfig->tileWidth;
	const int tileHeight = m_platformConfig->tileHeight;

	//Work backwards, find last placed game obj first
	for(TGameObjectPosMap::const_iterator itMap = m_gameObjects.begin(), endMap = m_gameObjects.end(); itMap != endMap && !gameObjectId; ++itMap)
	{
		for(std::vector<GameObjectMapEntry>::const_reverse_iterator itVec = itMap->second.rbegin(), endVec = itMap->second.rend(); itVec != endVec && !gameObjectId; ++itVec)
		{
			const int objWidth = (itVec->m_gameObject.GetDimensions().x > 0) ? itVec->m_gameObject.GetDimensions().x : itVec->m_size.x;
			const int objHeight = (itVec->m_gameObject.GetDimensions().y > 0) ? itVec->m_gameObject.GetDimensions().y : itVec->m_size.y;

			ion::Vector2i size(objWidth / tileWidth, objHeight / tileHeight);
			topLeft.x = itVec->m_gameObject.GetPosition().x / tileWidth;
			topLeft.y = itVec->m_gameObject.GetPosition().y / tileHeight;
			ion::Vector2i bottomRight = topLeft + size;

			if(x >= topLeft.x && y >= topLeft.y
				&& x < bottomRight.x && y < bottomRight.y)
			{
				gameObjectId = itVec->m_gameObject.GetId();
			}
		}
	}

	return gameObjectId;
}

GameObject* Map::FindGameObject(const std::string& name)
{
	for(TGameObjectPosMap::iterator itMap = m_gameObjects.begin(), endMap = m_gameObjects.end(); itMap != endMap; ++itMap)
	{
		for(std::vector<GameObjectMapEntry>::reverse_iterator itVec = itMap->second.rbegin(), endVec = itMap->second.rend(); itVec != endVec; ++itVec)
		{
			if(ion::string::CompareNoCase(itVec->m_gameObject.GetName(), name))
			{
				return &itVec->m_gameObject;
			}
		}
	}

	return NULL;
}

int Map::FindGameObjects(int x, int y, int width, int height, std::vector<const GameObjectMapEntry*>& gameObjects) const
{
	const int tileWidth = m_platformConfig->tileWidth;
	const int tileHeight = m_platformConfig->tileHeight;

	ion::Vector2i boundsMin(x * tileWidth, y * tileHeight);
	ion::Vector2i boundsMax((x * tileWidth) + width, (y * tileHeight) + height);

	for(TGameObjectPosMap::const_iterator it = m_gameObjects.begin(), end = m_gameObjects.end(); it != end; ++it)
	{
		const std::vector<GameObjectMapEntry>& gameObjectsOfType = it->second;

		for(int i = 0; i < gameObjectsOfType.size(); i++)
		{
			const GameObjectMapEntry& gameObject = gameObjectsOfType[i];

			const int objWidth = (gameObject.m_gameObject.GetDimensions().x > 0) ? gameObject.m_gameObject.GetDimensions().x : gameObject.m_size.x;
			const int objHeight = (gameObject.m_gameObject.GetDimensions().y > 0) ? gameObject.m_gameObject.GetDimensions().y : gameObject.m_size.y;

			if(ion::maths::BoxIntersectsBox(boundsMin, boundsMax, gameObject.m_gameObject.GetPosition(), gameObject.m_gameObject.GetPosition() + ion::Vector2i(objWidth, objHeight)))
			{
				gameObjects.push_back(&gameObject);
			}
		}
	}

	return gameObjects.size();
}

GameObject* Map::GetGameObject(GameObjectId gameObjectId)
{
	if(gameObjectId != InvalidGameObjectId)
	{
		for(TGameObjectPosMap::iterator itMap = m_gameObjects.begin(), endMap = m_gameObjects.end(); itMap != endMap; ++itMap)
		{
			for(std::vector<GameObjectMapEntry>::iterator it = itMap->second.begin(), end = itMap->second.end(); it != end; ++it)
			{
				if(it->m_gameObject.GetId() == gameObjectId)
				{
					return &it->m_gameObject;
				}
			}
		}
	}

	return NULL;
}

void Map::MoveGameObject(GameObjectId gameObjectId, int x, int y)
{
	if(gameObjectId != InvalidGameObjectId)
	{
		for(TGameObjectPosMap::iterator itMap = m_gameObjects.begin(), endMap = m_gameObjects.end(); itMap != endMap; ++itMap)
		{
			for(std::vector<GameObjectMapEntry>::iterator it = itMap->second.begin(), end = itMap->second.end(); it != end; ++it)
			{
				if(it->m_gameObject.GetId() == gameObjectId)
				{
					it->m_gameObject.SetPosition(ion::Vector2i(x, y));
					return;
				}
			}
		}
	}
}

void Map::RemoveGameObject(int x, int y)
{
	const int tileWidth = m_platformConfig->tileWidth;
	const int tileHeight = m_platformConfig->tileHeight;
	const int worldSpaceX = x * tileWidth;
	const int worldSpaceY = y * tileHeight;

	for(TGameObjectPosMap::iterator itMap = m_gameObjects.begin(), endMap = m_gameObjects.end(); itMap != endMap; ++itMap)
	{
		for(std::vector<GameObjectMapEntry>::reverse_iterator itVec = itMap->second.rbegin(), endVec = itMap->second.rend(); itVec != endVec; ++itVec)
		{
			const int width = (itVec->m_gameObject.GetDimensions().x > 0) ? itVec->m_gameObject.GetDimensions().x : itVec->m_size.x;
			const int height = (itVec->m_gameObject.GetDimensions().y > 0) ? itVec->m_gameObject.GetDimensions().y : itVec->m_size.y;

			ion::Vector2i size(width / tileWidth, height / tileHeight);
			if(size.x == 0)
				size.x = 1;
			if(size.y == 0)
				size.y = 1;
			ion::Vector2i topLeft = itVec->m_gameObject.GetPosition();
			ion::Vector2i bottomRight = topLeft + size;

			if(worldSpaceX >= topLeft.x && worldSpaceY >= topLeft.y
				&& worldSpaceX < bottomRight.x && worldSpaceY < bottomRight.y)
			{
				std::swap(*itVec, itMap->second.back());
				itMap->second.pop_back();

				if(itMap->second.size() == 0)
				{
					m_gameObjects.erase(itMap);
				}
				
				return;
			}
		}
	}
}

TGameObjectPosMap& Map::GetGameObjects()
{
	return m_gameObjects;
}

const TGameObjectPosMap& Map::GetGameObjects() const
{
	return m_gameObjects;
}

const TStampPosMap::const_iterator Map::StampsBegin() const
{
	return m_stamps.begin();
}

const TStampPosMap::const_iterator Map::StampsEnd() const
{
	return m_stamps.end();
}

TStampPosMap::iterator Map::StampsBegin()
{
	return m_stamps.begin();
}

TStampPosMap::iterator Map::StampsEnd()
{
	return m_stamps.end();
}

void Map::Export(const Project& project, std::vector<TileDesc>& tileMap) const
{
	//Copy tiles
	tileMap = m_tiles;

	//Blit stamps
	for (TStampPosMap::const_iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
	{
		if (const Stamp* stamp = project.GetStamp(it->m_id))
		{
			const ion::Vector2i& position = it->m_position;
			BakeStamp(tileMap, m_width, m_height, position.x, position.y, *stamp, it->m_flags);
		}
	}
}

void Map::Export(const Project& project, std::stringstream& stream) const
{
	//Copy tiles
	std::vector<TileDesc> tiles = m_tiles;

	//Blit stamps
	for(TStampPosMap::const_iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
	{
		if(const Stamp* stamp = project.GetStamp(it->m_id))
		{
			const ion::Vector2i& position = it->m_position;
			BakeStamp(tiles, m_width, m_height, position.x, position.y, *stamp, it->m_flags);
		}
	}

	//Use background tile if there is one, else use first tile
	u32 backgroundTileId = project.GetBackgroundTile();
	if(backgroundTileId == InvalidTileId)
	{
		backgroundTileId = 0;
	}

	//if(project.GetPlatformConfig().platform == ePlatformMegaDrive)
	{
		//Output to stream
		stream << std::hex << std::setfill('0') << std::uppercase;

		for(int y = 0; y < m_height; y++)
		{
			stream << "\tdc.w\t";

			for(int x = 0; x < m_width; x++)
			{
				//16 bit word:
				//-------------------
				//ABBC DEEE EEEE EEEE
				//-------------------
				//A = Low/high plane
				//B = Palette ID
				//C = Horizontal flip
				//D = Vertical flip
				//E = Tile ID

				const TileDesc& tileDesc = tiles[(y * m_width) + x];
				u8 paletteId = 0;

				//If blank tile, use background tile
				u32 tileId = (tileDesc.m_id == InvalidTileId) ? backgroundTileId : tileDesc.m_id;

				const Tile* tile = project.GetTileset().GetTile(tileId);
				ion::debug::Assert(tile, "Map::Export() - Invalid tile");

				//Generate components
				u16 tileIndex = tileId & 0x7FF;								//Bottom 11 bits = tile ID (index from 0)
				u16 flipH = (tileDesc.m_flags & eFlipX) ? 1 << 11 : 0;		//12th bit = Flip X flag
				u16 flipV = (tileDesc.m_flags & eFlipY) ? 1 << 12 : 0;		//13th bit = Flip Y flag
				u16 palette = (tile->GetPaletteId() & 0x3) << 13;			//14th+15th bits = Palette ID
				u16 plane = (tileDesc.m_flags & eHighPlane) ? 1 << 15 : 0;	//16th bit = High plane flag

				//Generate word
				u16 word = tileIndex | flipV | flipH | palette;

				stream << "0x" << std::setw(4) << (u32)word;

				if(x < (m_width - 1))
				{
					stream << ",";
				}
			}

			stream << std::endl;
		}

		stream << std::dec;
	}
	//else if(project.GetPlatformConfig().platform == ePlatformSNES)
	{
		//TODO: SNES export goes here
	}
}

void Map::Export(const Project& project, ion::io::File& file) const
{
	//Copy tiles
	std::vector<TileDesc> tiles = m_tiles;

	//Blit stamps
	for(TStampPosMap::const_iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
	{
		const Stamp* stamp = project.GetStamp(it->m_id);
		if(stamp)
		{
			const ion::Vector2i& position = it->m_position;
			BakeStamp(tiles, m_width, m_height, position.x, position.y, *stamp, it->m_flags);
		}
	}

	//Use background tile if there is one, else use first tile
	u32 backgroundTileId = project.GetBackgroundTile();
	if(backgroundTileId == InvalidTileId)
	{
		backgroundTileId = 0;
	}

	//if(project.GetPlatformConfig().platform == ePlatformMegaDrive)
	{
		for(int y = 0; y < m_height; y++)
		{
			for(int x = 0; x < m_width; x++)
			{
				//16 bit word:
				//-------------------
				//ABBC DEEE EEEE EEEE
				//-------------------
				//A = Low/high plane
				//B = Palette ID
				//C = Horizontal flip
				//D = Vertical flip
				//E = Tile ID

				const TileDesc& tileDesc = tiles[(y * m_width) + x];
				u8 paletteId = 0;

				//If blank tile, use background tile
				u32 tileId = (tileDesc.m_id == InvalidTileId) ? backgroundTileId : tileDesc.m_id;

				const Tile* tile = project.GetTileset().GetTile(tileId);
				ion::debug::Assert(tile, "Map::Export() - Invalid tile");

				//Generate components
				u16 tileIndex = tileId & 0x7FF;								//Bottom 11 bits = tile ID (index from 0)
				u16 flipH = (tileDesc.m_flags & eFlipX) ? 1 << 11 : 0;		//12th bit = Flip X flag
				u16 flipV = (tileDesc.m_flags & eFlipY) ? 1 << 12 : 0;		//13th bit = Flip Y flag
				u16 palette = (tile->GetPaletteId() & 0x3) << 13;			//14th+15th bits = Palette ID
				u16 plane = (tileDesc.m_flags & eHighPlane) ? 1 << 15 : 0;	//16th bit = High plane flag

				//Generate word
				u16 word = tileIndex | flipV | flipH | palette | plane;

				//Endian flip
				ion::memory::EndianSwap(word);

				//Write
				file.Write(&word, sizeof(u16));
			}
		}
	}
	//else if(project.GetPlatformConfig().platform == ePlatformSNES)
	{
		//TODO: SNES export goes here
	}
}

void Map::GenerateBlocks(const Project& project, int blockWidth, int blockHeight)
{
	//Align map size to block size
	int mapWidthBlockAligned = GetBlockAlignedWidth(blockWidth);
	int mapHeightBlockAligned = GetBlockAlignedHeight(blockHeight);

	//Use background tile if there is one, else use first tile
	u32 backgroundTileId = project.GetBackgroundTile();
	if(backgroundTileId == InvalidTileId)
	{
		backgroundTileId = 0;
	}

	//Copy tiles (using new aligned map size)
	std::vector<TileDesc> tiles;
	tiles.resize(mapWidthBlockAligned * mapHeightBlockAligned);
	std::fill(tiles.begin(), tiles.end(), TileDesc(backgroundTileId, 0));

	for (int x = 0; x < mapWidthBlockAligned; x++)
	{
		for (int y = 0; y < mapHeightBlockAligned; y++)
		{
			//Clamp aligned size to original size (repeats last tile)
			int clampedX = ion::maths::Clamp(x, 0, m_width - 1);
			int clampedY = ion::maths::Clamp(y, 0, m_height - 1);

			int index = (y * mapWidthBlockAligned) + x;
			int indexClamped = (clampedY * m_width) + clampedX;

			tiles[index] = m_tiles[indexClamped];
		}
	}

	//Blit stamps
	for(TStampPosMap::const_iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
	{
		const Stamp* stamp = project.GetStamp(it->m_id);
		if(stamp)
		{
			const ion::Vector2i& position = it->m_position;
			BakeStamp(tiles, mapWidthBlockAligned, mapHeightBlockAligned, position.x, position.y, *stamp, it->m_flags);
		}
	}

	//Split map into NxN blocks
	int widthBlocks = mapWidthBlockAligned / blockWidth;
	int heightBlocks = mapHeightBlockAligned / blockHeight;

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

					int tileOffset = (y * mapWidthBlockAligned) + x;

					if (tiles[tileOffset].m_id == InvalidTileId)
					{
						tiles[tileOffset].m_id = backgroundTileId;
					}

					block.m_tiles.push_back(tiles[tileOffset]);
				}
			}
		}
	}
}

std::vector<Map::Block>& Map::GetBlocks()
{
	return m_blocks;
}

const std::vector<Map::Block>& Map::GetBlocks() const
{
	return m_blocks;
}

void Map::ExportBlockMap(const Project& project, std::stringstream& stream, int blockWidth, int blockHeight) const
{
	int widthBlocks = GetWidthBlocks(blockWidth);
	int heightBlocks = GetHeightBlocks(blockHeight);

	//Export block map
	stream << std::endl;
	stream << "Block_Map:" << std::endl;

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

void Map::ExportBlockMap(const Project& project, ion::io::File& file, int blockWidth, int blockHeight, std::vector<u32>& colOffsets) const
{
	if(m_blocks.size() > 0)
	{
		int widthBlocks = GetWidthBlocks(blockWidth);
		int heightBlocks = GetHeightBlocks(blockHeight);

		//Compression buffers
		std::vector<u16> uncompressedData;
		std::vector<u8> compressedData;

		//Create RLE compressor, use 3 bits for run size
		const int runSizeBits = 3;
		ion::io::CompressorRLE<u16, runSizeBits> compressor;

		int colOffset = 0;

		//Export block map
		for(int blockX = 0; blockX < widthBlocks; blockX++)
		{
			for(int blockY = 0; blockY < heightBlocks; blockY++)
			{
				int blockId = (blockY * widthBlocks) + blockX;
				const Block& block = m_blocks[blockId];

				u16 word = block.uniqueIndex;

				//Write to buffer
				uncompressedData.push_back(word);
			}

			//Add col offset
			colOffsets.push_back(colOffset);

			//Compress col
			int compressedSize = compressor.Compress(uncompressedData, compressedData);

			//Write col
			file.Write(compressedData.data(), compressedData.size());

			//Advance col
			colOffset += compressedSize;
			
			//Sanity check
			std::vector<u16> decompressedData;
			ion::io::DecompressorRLE<u16, runSizeBits> decompressor;
			decompressor.Decompress(compressedData, decompressedData);
			ion::debug::Assert(decompressedData == uncompressedData, "Block map compression error");

			//Clear buffers
			uncompressedData.clear();
			compressedData.clear();
		}
	}
}

void Map::ExportStampMap(const Project& project, std::stringstream& stream) const
{
	//Assign indices to stampIds
	std::map<StampId, u32> indexMap;

	int index = 0;
	for(TStampMap::const_iterator it = project.StampsBegin(), end = project.StampsEnd(); it != end; ++it, ++index)
	{
		indexMap[it->first] = index;
	}

	//Sort by X and Y coord
	std::vector<std::pair<const StampMapEntry*, s32>> sortedX;
	std::vector<std::pair<const StampMapEntry*, s32>> sortedY;

	for(TStampPosMap::const_iterator it = m_stamps.begin(), end = m_stamps.end(); it != end; ++it)
	{
		if(const Stamp* stamp = project.GetStamp(it->m_id))
		{
			const ion::Vector2i& position = it->m_position;

			//TODO: Support negative placements
			if(position.x >= 0 && position.y >= 0)
			{
				sortedX.push_back(std::make_pair(&(*it), position.x));
				sortedY.push_back(std::make_pair(&(*it), position.y));
			}
		}
	}

	std::sort(sortedX.begin(), sortedX.end(), [](const std::pair<const StampMapEntry*, s32>& lhs, const std::pair<const StampMapEntry*, s32>& rhs) { return lhs.second < rhs.second; });
	std::sort(sortedY.begin(), sortedY.end(), [](const std::pair<const StampMapEntry*, s32>& lhs, const std::pair<const StampMapEntry*, s32>& rhs) { return lhs.second < rhs.second; });

	//Export count
	stream << "stampmap_" << project.GetName() << "_count equ 0x" << HEX4((int)sortedX.size()) << std::endl;

	stream << std::endl;
	
	//Export X-sorted table
	stream << "stampmap_" << project.GetName() << "_x_table:" << std::endl;

	for(int i = 0; i < sortedX.size(); i++)
	{
		stream << "stampentry_x_" << project.GetName() << "_" << i << "_xpos:\tdc.w 0x" << HEX4((int)sortedX[i].first->m_position.x) << std::endl;
		stream << "stampentry_x_" << project.GetName() << "_" << i << "_ypos:\tdc.w 0x" << HEX4((int)sortedX[i].first->m_position.y) << std::endl;
		stream << "stampentry_x_" << project.GetName() << "_" << i << "_idx:\tdc.w 0x" << HEX4((int)indexMap[sortedX[i].first->m_id]) << std::endl;
		stream << "stampentry_x_" << project.GetName() << "_" << i << "_flags:\tdc.w 0x" << HEX4((int)sortedX[i].first->m_flags) << std::endl;
	}

	stream << std::endl;

	//Export Y-sorted table
	stream << "stampmap_" << project.GetName() << "_y_table:" << std::endl;

	for(int i = 0; i < sortedY.size(); i++)
	{
		stream << "stampentry_y_" << project.GetName() << "_" << i << "_xpos:\tdc.w 0x" << HEX4((int)sortedY[i].first->m_position.x) << std::endl;
		stream << "stampentry_y_" << project.GetName() << "_" << i << "_ypos:\tdc.w 0x" << HEX4((int)sortedY[i].first->m_position.y) << std::endl;
		stream << "stampentry_y_" << project.GetName() << "_" << i << "_idx:\tdc.w 0x" << HEX4((int)indexMap[sortedY[i].first->m_id]) << std::endl;
		stream << "stampentry_y_" << project.GetName() << "_" << i << "_flags:\tdc.w 0x" << HEX4((int)sortedY[i].first->m_flags) << std::endl;
	}
}

void Map::ExportStampMap(const Project& project, ion::io::File& file) const
{

}

bool Map::Block::operator ==(const Block& rhs) const
{
	return m_tiles == rhs.m_tiles;
}

void Map::Block::Export(const Project& project, std::stringstream& stream, int blockWidth, int blockHeight)
{
	//Output to stream
	stream << std::hex << std::setfill('0') << std::uppercase;

	for(int y = 0; y < blockHeight; y++)
	{
		stream << "\tdc.w\t";

		for(int x = 0; x < blockWidth; x++)
		{
			//16 bit word:
			//-------------------
			//ABBC DEEE EEEE EEEE
			//-------------------
			//A = Low/high plane
			//B = Palette ID
			//C = Horizontal flip
			//D = Vertical flip
			//E = Tile ID

			const TileDesc& tileDesc = m_tiles[(y * blockWidth) + x];
			u8 paletteId = 0;

			const Tile* tile = project.GetTileset().GetTile(tileDesc.m_id);
			ion::debug::Assert(tile, "Map::Export() - Invalid tile");

			//Generate components
			u16 tileIndex = tileDesc.m_id & 0x7FF;								//Bottom 11 bits = tile ID (index from 0)
			u16 flipH = (tileDesc.m_flags & eFlipX) ? 1 << 11 : 0;		//12th bit = Flip X flag
			u16 flipV = (tileDesc.m_flags & eFlipY) ? 1 << 12 : 0;		//13th bit = Flip Y flag
			u16 palette = (tile->GetPaletteId() & 0x3) << 13;			//14th+15th bits = Palette ID
			u16 plane = (tileDesc.m_flags & eHighPlane) ? 1 << 15 : 0;	//16th bit = High plane flag

			//Generate word
			u16 word = tileIndex | flipV | flipH | palette | plane;

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

void Map::Block::Export(const Project& project, ion::io::File& file, int blockWidth, int blockHeight)
{
	//Output to file
	for(int y = 0; y < blockHeight; y++)
	{
		for(int x = 0; x < blockWidth; x++)
		{
			//16 bit word:
			//-------------------
			//ABBC DEEE EEEE EEEE
			//-------------------
			//A = Low/high plane
			//B = Palette ID
			//C = Horizontal flip
			//D = Vertical flip
			//E = Tile ID

			const TileDesc& tileDesc = m_tiles[(y * blockWidth) + x];
			u8 paletteId = 0;

			const Tile* tile = project.GetTileset().GetTile(tileDesc.m_id);
			ion::debug::Assert(tile, "Map::Export() - Invalid tile");

			//Generate components
			u16 tileIndex = tileDesc.m_id & 0x7FF;						//Bottom 11 bits = tile ID (index from 0)
			u16 flipH = (tileDesc.m_flags & eFlipX) ? 1 << 11 : 0;		//12th bit = Flip X flag
			u16 flipV = (tileDesc.m_flags & eFlipY) ? 1 << 12 : 0;		//13th bit = Flip Y flag
			u16 palette = (tile->GetPaletteId() & 0x3) << 13;			//14th+15th bits = Palette ID
			u16 plane = (tileDesc.m_flags & eHighPlane) ? 1 << 15 : 0;	//16th bit = High plane flag

			//Generate word
			u16 word = tileIndex | flipV | flipH | palette | plane;

			//Endian flip
			ion::memory::EndianSwap(word);

			//Write
			file.Write(&word, sizeof(u16));
		}
	}
}