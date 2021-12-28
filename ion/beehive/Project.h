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

#include <ion/core/io/Archive.h>
#include <string>
#include <vector>
#include <set>

#include "Types.h"
#include "PlatformConfig.h"
#include "Map.h"
#include "Palette.h"
#include "SpriteSheet.h"
#include "Actor.h"
#include "Stamp.h"
#include "Tile.h"
#include "Tileset.h"
#include "CollisionMap.h"
#include "TerrainTile.h"
#include "TerrainTileset.h"
#include "TerrainStamp.h"
#include "GameObject.h"

typedef std::map<MapId, Map> TMapMap;
typedef std::map<CollisionMapId, CollisionMap> TCollisionMapMap;
typedef std::map<StampId, Stamp> TStampMap;
typedef std::map<TerrainStampId, TerrainStamp> TTerrainStampMap;
typedef std::map<ActorId, Actor> TActorMap;
typedef std::map<AnimationId, Animation> TAnimationMap;
typedef std::map<GameObjectTypeId, GameObjectType> TGameObjectTypeMap;

class Project
{
	//TODO: Add/remove palettes
	static const int s_maxPalettes = 4;

public:
	enum BMPImportFlags
	{
		eTileImportClearPalettes		= (1 << 0),
		eTileImportClearTiles			= (1 << 1),
		eTileImportClearMap				= (1 << 2),
		eTileImportDrawToMap			= (1 << 3),
		eTileImportToStamp				= (1 << 4),
		eTileImportReplaceStamp			= (1 << 5),
		eTileImportToSpriteSheet		= (1 << 6),
		eTileImportWholePalette			= (1 << 7),
		eTileImportInsertBGTile			= (1 << 8),
		eTileImportOnlyExistingStamps	= (1 << 9),
		eTileImportNoDuplicateTileCheck	= (1 << 10),
	};

	enum class ExportFormat
	{
		Text,
		Binary,
		BinaryCompressed,
		Beehive
	};

	enum class IncludeExportFlags
	{
		None,
		DebugOnly,
		ReleaseOnly
	};

	struct IncludeFile
	{
		std::string label;
		std::string filename;
		IncludeExportFlags flags;
	};

	Project(PlatformConfig& defaultPatformConfig);

	//Load/save project
	bool Load(const std::string& filename);
	bool Save(const std::string& filename);

	//Get platform config
	const PlatformConfig& GetPlatformConfig() const { return m_platformConfig; }
	PlatformConfig& GetPlatformConfig() { return m_platformConfig; }

	//Get/set name
	const std::string& GetName() const { return m_name; }
	void SetName(const std::string& name) { m_name = name; }

	//Get filename
	const std::string& GetFilename() { return m_filename; }

	//Clear project (palettes, tiles, map)
	void Clear();

	//Maps
	MapId CreateMap();
	MapId CreateMap(const Map& copyFrom);
	MapId CreateMap(MapId mapId);
	void DeleteMap(MapId mapId);
	Map& GetMap(MapId mapId);
	const Map& GetMap(MapId mapId) const;
	Map* FindMap(const std::string name);
	MapId FindMapId(const std::string name);
	Map& GetEditingMap();
	MapId GetEditingMapId() const;
	void SetEditingMap(MapId mapId);
	const TMapMap::const_iterator MapsBegin() const;
	const TMapMap::const_iterator MapsEnd() const;
	TMapMap::iterator MapsBegin();
	TMapMap::iterator MapsEnd();
	int GetMapCount() const;

	//Collision maps
	CollisionMapId CreateCollisionMap(CollisionMapId collisionMapId);
	CollisionMapId CreateCollisionMap(CollisionMapId collisionMapId, const CollisionMap& copyFrom);
	CollisionMapId CreateCollisionMap(CollisionMapId collisionMapId, int width, int height);
	void DeleteCollisionMap(CollisionMapId collisionMapId);
	CollisionMap& GetCollisionMap(CollisionMapId collisionMapId);
	const CollisionMap& GetCollisionMap(CollisionMapId collisionMapId) const;
	CollisionMap& GetEditingCollisionMap();
	CollisionMapId GetEditingCollisionMapId() const;
	void SetEditingCollisionMap(CollisionMapId collisionMapId);
	const TCollisionMapMap::const_iterator CollisionMapsBegin() const;
	const TCollisionMapMap::const_iterator CollisionMapsEnd() const;
	int GetCollisionMapCount() const;

	//Get tileset
	Tileset& GetTileset() { return m_tileset; }
	const Tileset& GetTileset() const { return m_tileset; }

	//Get terrain tileset
	TerrainTileset& GetTerrainTileset() { return m_terrainTileset; }
	const TerrainTileset& GetTerrainTileset() const { return m_terrainTileset; }

	//Get palette
	Palette* GetPalette(PaletteId paletteId) { return &m_palettes[paletteId]; }
	const Palette* GetPalette(PaletteId paletteId) const { return &m_palettes[paletteId]; }
	int GetNumPalettes() const { return s_maxPalettes; }

	//Palette slots
	int AddPaletteSlot(Palette& palette);
	Palette* GetPaletteSlot(int slotIndex);
	const Palette* GetPaletteSlot(int slotIndex) const;
	int GetNumPaletteSlots() const;
	void SetActivePaletteSlot(PaletteId paletteId, int slotIndex);
	void ExportPaletteSlots(const std::string& filename, ExportFormat format);
	void ImportPaletteSlots(const std::string& filename);
	void SetBackgroundColour(u8 paletteId, u8 colourIdx);
	void SwapPaletteEntries(u8 paletteId, u8 colourA, u8 colourB);
	void MergePaletteEntries(u8 paletteId, u8 mergeFromIdx, u8 mergeToIdx);

	//Tiles
	void DeleteTile(TileId tileId);
	void SwapTiles(TileId tileId1, TileId tileId2);
	void SetBackgroundTile(TileId tileId);
	TileId GetBackgroundTile() const { return m_backgroundTile; }
	int CleanupTiles();

	//Actors
	ActorId CreateActor();
	void DeleteActor(ActorId actorId);
	Actor* GetActor(ActorId actorId);
	const Actor* GetActor(ActorId actorId) const;
	Actor* FindActor(const std::string& name);
	ActorId FindActorId(const std::string& name) const;
	const TActorMap& GetActors() const;
	const TActorMap::const_iterator ActorsBegin() const;
	const TActorMap::const_iterator ActorsEnd() const;
	int GetActorCount() const;
	bool ExportActors(const std::string& filename, ExportFormat format);
	bool ImportActors(const std::string& filename, bool clearExisting);

	//Animations
	AnimationId CreateAnimation();
	void DeleteAnimation(AnimationId animationId);
	Animation* GetAnimation(AnimationId animationId);
	const Animation* GetAnimation(AnimationId animationId) const;
	const TAnimationMap::const_iterator AnimationsBegin() const;
	const TAnimationMap::const_iterator AnimationsEnd() const;
	int GetAnimationCount() const;
	bool ExportAnimations(const std::string& filename);
	bool ImportAnimations(const std::string& filename, bool clearExisting);

	//Stamps
	StampId AddStamp(int width, int height);
	StampId AddStamp(Stamp* stamp);
	void DeleteStamp(StampId stampId);
	Stamp* GetStamp(StampId stampId);
	const Stamp* GetStamp(StampId stampId) const;
	Stamp* FindStamp(const std::string& name);
	StampId FindDuplicateStamp(Stamp* stamp) const;
	const TStampMap::const_iterator StampsBegin() const;
	const TStampMap::const_iterator StampsEnd() const;
	int GetStampCount() const;
	int CleanupStamps();
	int CompactStampIds();
	void SubstituteStamp(StampId stampToReplace, StampId substitution);
	void SortStampTilesSequentially(Stamp* stamp);
	void SetBackgroundStamp(StampId stampId) { m_backgroundStamp = stampId; }
	StampId GetBackgroundStamp() const { return m_backgroundStamp; }

	//Terrain tiles
	void DeleteTerrainTile(TerrainTileId tileId);
	void SwapTerrainTiles(TerrainTileId tileId1, TerrainTileId tileId2);
	void SetDefaultTerrainTile(TerrainTileId tileId);
	TerrainTileId GetDefaultTerrainTile() const { return m_defaultTerrainTile; }
	int CleanupTerrainTiles(bool prompt);

	//Terrain stamps
	TerrainStampId AddTerrainStamp(int width, int height);
	TerrainStampId AddTerrainStamp(TerrainStamp* terrainStamp);
	void DeleteTerrainStamp(TerrainStampId terrainStampId);
	TerrainStamp* GetTerrainStamp(TerrainStampId terrainStampId);
	const TerrainStamp* GetTerrainStamp(TerrainStampId terrainStampId) const;
	TerrainStampId FindDuplicateTerrainStamp(TerrainStamp* terrainStamp) const;
	const TTerrainStampMap::const_iterator TerrainStampsBegin() const;
	const TTerrainStampMap::const_iterator TerrainStampsEnd() const;
	int GetTerrainStampCount() const;
	int CleanupTerrainStamps();

	//Terrain generation from bezier paths
	bool GenerateTerrainFromBeziers();
	bool GenerateTerrainFromBeziers_HeightsOnly(int granularity);

	//Terrain generation from graphic tiles
	void GenerateTerrain(const std::vector<ion::Vector2i>& graphicTiles);

	//Game object types
	GameObjectTypeId AddGameObjectType();
	void RemoveGameObjectType(GameObjectTypeId typeId);
	GameObjectType* GetGameObjectType(GameObjectTypeId typeId);
	const GameObjectType* GetGameObjectType(GameObjectTypeId typeId) const;
	GameObjectType* FindGameObjectType(const std::string& name);
	const TGameObjectTypeMap& GetGameObjectTypes() const;
	bool ExportGameObjectTypes(const std::string& filename, ExportFormat format, bool minimal);
	bool ImportGameObjectTypes(const std::string& filename, bool clearExisting);

	//Set current colour used for editing
	void SetPaintColour(u8 colourIdx);
	u8 GetPaintColour() const;

	//Set current collision tile used for painting
	void SetPaintTerrainTile(TerrainTileId tile);
	TerrainTileId GetPaintTerrainTile() const;

	//Set current tile used for painting
	void SetPaintTile(TileId tile);
	TileId GetPaintTile() const;

	//Set current tile used for erasing
	void SetEraseTile(TileId tile);
	TileId GetEraseTile() const;

	//Set current stamp used for painting
	void SetPaintStamp(StampId stamp);
	StampId GetPaintStamp() const;

	//Set current game object type for placing
	void SetPaintGameObjectType(GameObjectTypeId typeId, GameObjectArchetypeId archetypeId = InvalidGameObjectArchetypeId);
	GameObjectTypeId GetPaintGameObjectType() const;
	GameObjectArchetypeId GetPaintGameObjectArchetype() const;

	//Grid
	int GetGridSize() const { return m_gridSize; }
	void SetGridSize(int gridSize) { m_gridSize = gridSize; }
	bool GetShowGrid() const { return m_showGrid; }
	void SetShowGrid(bool show) { m_showGrid = show; }
	bool GetGridSnap() const { return m_snapToGrid; }
	void SetGridSnap(bool snap) { m_snapToGrid = snap; }

	//Stamp outlines
	bool GetShowStampOutlines() const { return m_showStampOutlines; }
	void SetShowStampOutlines(bool show) { m_showStampOutlines = show; }

	//Display frame
	bool GetShowDisplayFrame() const { return m_showDisplayFrame; }
	void SetShowDisplayFrame(bool show) { m_showDisplayFrame = show; }

	//Collision
	bool GetShowCollision() const { return m_showCollision; }
	void SetShowCollision(bool show) { m_showCollision = show; }

	void InvalidateMap(bool invalidate) { m_mapInvalidated = invalidate; }
	void InvalidateTiles(bool invalidate) { m_tilesInvalidated = invalidate; }
	void InvalidateTerrainTiles(bool invalidate) { m_terrainTilesInvalidated = invalidate; }
	void InvalidateTerrainBeziers(bool invalidate) { m_terrainBeziersInvalidated = invalidate; }
	void InvalidateStamps(bool invalidate) { m_stampsInvalidated = invalidate; }
	void InvalidateCamera(bool invalidate) { m_cameraInvalidated = invalidate; }
	void InvalidateBlocks(bool invalidate) { m_blocksInvalidated = invalidate; }
	bool MapIsInvalidated() const { return m_mapInvalidated; }
	bool TilesAreInvalidated() const { return m_tilesInvalidated; }
	bool TerrainTilesAreInvalidated() const { return m_terrainTilesInvalidated; }
	bool TerrainBeziersAreInvalidated() const { return m_terrainBeziersInvalidated; }
	bool StampsAreInvalidated() const { return m_stampsInvalidated; }
	bool CameraIsInvalidated() const { return m_cameraInvalidated; }
	bool BlocksAreInvalidated() const { return m_blocksInvalidated; }

	//Import bitmap
	bool ImportBitmap(const std::string& filename, u32 importFlags, u32 paletteBits, Stamp* stamp = NULL);

	//Export
	void WriteIncludeFile(const std::string& projectDir, const std::string& exportDir, const std::string& includeFilename, const std::vector<IncludeFile>& filenames, bool generateLabel) const;
	void WriteFileHeader(std::stringstream& stream) const;

	bool ExportPalettes(const std::string& filename, ExportFormat format);
	bool ExportTiles(const std::string& filename, ExportFormat format);
	bool ExportStampAnims(const std::string& filename, ExportFormat format) const;
	bool ExportTerrainTiles(const std::string& filename, ExportFormat format);
	bool ExportSpriteSheets(const std::string& directory, ExportFormat format);
	bool ExportSpriteAnims(const std::string& directory, ExportFormat format) const;
	bool ExportSpritePalettes(const std::string& directory) const;
	bool ExportMapBitmaps(const std::string& directory) const;
	bool ExportStampBitmaps(const std::string& directory) const;

	bool ExportMap(MapId mapId, const std::string& filename, ExportFormat format) const;
	bool ExportBlocks(const std::string& filename, ExportFormat format, int blockWidth, int blockHeight);
	bool ExportBlockMap(MapId mapId, const std::string& filename, ExportFormat format, int blockWidth, int blockHeight) const;
	bool ExportStamps(const std::string& filename, ExportFormat format);
	bool ExportStampMap(MapId mapId, const std::string& filename, ExportFormat format);
	bool ExportCollisionMap(MapId mapId, const std::string& filename, ExportFormat format);
	bool ExportTerrainBlocks(const std::string& filename, ExportFormat format, int blockWidth, int blockHeight);
	bool ExportTerrainBlockMap(MapId mapId, const std::string& filename, ExportFormat format, int blockWidth, int blockHeight);
	bool ExportTerrainAngles(const std::string& filename, ExportFormat format);
	bool ExportSceneAnimations(MapId mapId, const std::string& filename, ExportFormat format);
	bool ExportGameObjects(MapId mapId, const std::string& filename, ExportFormat format);

	//Serialise
	void Serialise(ion::io::Archive& archive);

	//Last used export filenames
	struct ExportFilenames
	{
		std::string palettes;
		std::string tileset;
		std::string blocks;
		std::string stamps;
		std::string stampAnims;
		std::string terrainTiles;
		std::string terrainBlocks;
		std::string terrainAngles;
		std::string gameObjTypes;
		std::string spriteSheets;
		std::string spriteAnims;
		std::string spritePalettes;

		bool palettesExportEnabled = false;
		bool tilesetExportEnabled = false;
		bool blockExportEnabled = false;
		bool stampsExportEnabled = false;
		bool stampAnimsExportEnabled = false;
		bool terrainTilesExportEnabled = false;
		bool terrainBlockExportEnabled = false;
		bool terrainAngleExportEnabled = false;
		bool gameObjTypesExportEnabled = false;
		bool spriteSheetsExportEnabled = false;
		bool spriteAnimsExportEnabled = false;
		bool spritePalettesExportEnabled = false;

		ExportFormat exportFormat = ExportFormat::BinaryCompressed;
		bool exportCompressed = true;

		void Serialise(ion::io::Archive& archive)
		{
			archive.Serialise(palettesExportEnabled, "palettesExportEnabled");
			archive.Serialise(tilesetExportEnabled, "tilesetExportEnabled");
			archive.Serialise(blockExportEnabled, "blockExportEnabled");
			archive.Serialise(stampsExportEnabled, "stampsExportEnabled");
			archive.Serialise(stampAnimsExportEnabled, "stampAnimsExportEnabled");
			archive.Serialise(terrainTilesExportEnabled, "terrainTilesExportEnabled");
			archive.Serialise(terrainBlockExportEnabled, "terrainBlockExportEnabled");
			archive.Serialise(terrainAngleExportEnabled, "terrainAngleExportEnabled");
			archive.Serialise(gameObjTypesExportEnabled, "gameObjTypesExportEnabled");
			archive.Serialise(spriteSheetsExportEnabled, "spriteSheetsExportEnabled");
			archive.Serialise(spriteAnimsExportEnabled, "spriteAnimsExportEnabled");
			archive.Serialise(spritePalettesExportEnabled, "spritePalettesExportEnabled");
			archive.Serialise((int&)exportFormat, "exportFormat");

			archive.Serialise(palettes, "exportFNamePalettes");
			archive.Serialise(tileset, "exportFNameTileset");
			archive.Serialise(blocks, "exportFNameBlocks");
			archive.Serialise(stamps, "exportFNameStamps");
			archive.Serialise(stampAnims, "exportFNameStampAnims");
			archive.Serialise(terrainTiles, "exportFNameTerrainTiles");
			archive.Serialise(terrainBlocks, "exportFNameTerrainBlocks");
			archive.Serialise(terrainAngles, "exportFNameTerrainAngles");
			archive.Serialise(gameObjTypes, "exportFNameGameObjTypes");
			archive.Serialise(spriteSheets, "exportDirSpriteSheets");
			archive.Serialise(spriteAnims, "exportDirSpriteAnims");
			archive.Serialise(spritePalettes, "exportDirSpritePalettes");
		}
	};

	//Project settings
	struct Settings
	{
		std::string Get(const std::string& key) const
		{
			const std::map<std::string, std::string>::const_iterator it = settings.find(key);
			return (it == settings.end()) ? "" :  it->second;
		}

		void Set(const std::string& key, const std::string& value)
		{
			settings[key] = value;
		}

		void Serialise(ion::io::Archive& archive)
		{
			archive.Serialise(settings, "settings");

			if (archive.GetDirection() == ion::io::Archive::Direction::In)
			{
				//Legacy
				std::string engineRootDir;
				std::string projectRootDir;
				std::string gameObjectsExternalFile;
				std::string spriteActorsExternalFile;
				std::string sceneExportDir;
				std::string spritesExportDir;
				std::string spriteAnimsExportDir;
				std::string spritePalettesExportDir;

				archive.Serialise(engineRootDir, "engineRootDir");
				archive.Serialise(projectRootDir, "projectRootDir");
				archive.Serialise(sceneExportDir, "sceneExportDir");
				archive.Serialise(spritesExportDir, "spritesExportDir");
				archive.Serialise(spriteAnimsExportDir, "spriteAnimsExportDir");
				archive.Serialise(spritePalettesExportDir, "spritePalettesExportDir");
				archive.Serialise(gameObjectsExternalFile, "gameObjectsExternalFile");
				archive.Serialise(spriteActorsExternalFile, "spriteActorsExternalFile");

				if (engineRootDir.size()) { settings["engineRootDir"] = engineRootDir; }
				if (projectRootDir.size()) { settings["projectRootDir"] = projectRootDir; }
				if (gameObjectsExternalFile.size()) { settings["gameObjectsExternalFile"] = gameObjectsExternalFile; }
				if (spriteActorsExternalFile.size()) { settings["spriteActorsExternalFile"] = spriteActorsExternalFile; }
				if (sceneExportDir.size()) { settings["sceneExportDir"] = sceneExportDir; }
				if (spritesExportDir.size()) { settings["spritesExportDir"] = spritesExportDir; }
				if (spriteAnimsExportDir.size()) { settings["spriteAnimsExportDir"] = spriteAnimsExportDir; }
				if (spritePalettesExportDir.size()) { settings["spritePalettesExportDir"] = spritePalettesExportDir; }
			}
		}

	private:
		std::map<std::string, std::string> settings;
	};
	
	ExportFilenames m_exportFilenames;
	Settings m_settings;

private:
	//Find palette matching 8x8 colour grid
	bool FindPalette(Colour* pixels, u32 useablePalettes, PaletteId& paletteId, PaletteId& closestPalette, int& closestColourCount) const;
	bool ImportPalette(Colour* pixels, Palette& palette);
	bool MergePalettes(Palette& dest, const Palette& source);

	//Collapse palette slots to palettes in use
	void CollapsePaletteSlots();

	//Generate heighmap from graphic tile
	void GenerateHeightMap(const Tile& graphicTile, std::vector<u8>& heightMap) const;
	void GenerateCeilingMap(const Tile& graphicTile, std::vector<u8>& ceilingMap) const;

	//Get tile at position (including on stamps)
	TileId GetTileAtPosition(const ion::Vector2i& position);

	//Platform config
	PlatformConfig m_platformConfig;

	//Project name
	std::string m_name;

	//Filename
	std::string m_filename;

	//Tileset
	Tileset m_tileset;

	//Terrain tileset
	TerrainTileset m_terrainTileset;

	//Terrain stamps
	TTerrainStampMap m_terrainStamps;
	TerrainStampId m_nextFreeTerrainStampId;

	//Map
	TMapMap m_maps;

	//Collision map
	TCollisionMapMap m_collisionMaps;

	//Active palettes
	std::vector<Palette> m_palettes;

	//Saved palette slots
	std::vector<Palette> m_paletteSlots;

	//Stamps
	TStampMap m_stamps;
	StampId m_nextFreeStampId;

	//Actors
	std::map<ActorId, Actor> m_actors;

	//Animations
	TAnimationMap m_animations;

	//Game object types
	TGameObjectTypeMap m_gameObjectTypes;
	GameObjectTypeId m_nextFreeGameObjectTypeId;

	//Colour used for painting
	u8 m_paintColour;

	//Collision tile used for painting
	TerrainTileId m_paintTerrainTile;

	//Tile used for painting
	TileId m_paintTile;

	//Stamp used for painting
	StampId m_paintStamp;

	//Tile used for erasing
	TileId m_eraseTile;

	//Game object used for placing
	GameObjectTypeId m_paintGameObjectType;
	GameObjectArchetypeId m_paintGameObjectArchetype;

	//Background tile (replaces InvalidTileId on export)
	TileId m_backgroundTile;

	//Background stamp (replaces InvalidStampId on export)
	StampId m_backgroundStamp;

	//Default collision tile (replaces InvalidTerrainTileId on export)
	TerrainTileId m_defaultTerrainTile;

	//Currently editing map id
	MapId m_editingMapId;

	//Currently editing collision map id
	CollisionMapId m_editingCollisionMapId;

	//Grid
	int m_gridSize;
	bool m_showGrid;
	bool m_snapToGrid;

	//Stamp outlines
	bool m_showStampOutlines;

	//Display frame
	bool m_showDisplayFrame;

	//Collision
	bool m_showCollision;

	//Map needs redraw
	bool m_mapInvalidated;
	bool m_tilesInvalidated;
	bool m_terrainTilesInvalidated;
	bool m_terrainBeziersInvalidated;
	bool m_stampsInvalidated;
	bool m_cameraInvalidated;
	bool m_blocksInvalidated;
};