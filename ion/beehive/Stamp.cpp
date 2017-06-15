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

#include "Stamp.h"
#include "Project.h"

#include <core/memory/Memory.h>
#include <core/memory/Endian.h>
#include <core/string/String.h>
#include <core/cryptography/Hash.h>

#define HEX1(val) std::hex << std::setfill('0') << std::setw(1) << std::uppercase << (int)val
#define HEX2(val) std::hex << std::setfill('0') << std::setw(2) << std::uppercase << (int)val
#define HEX4(val) std::hex << std::setfill('0') << std::setw(4) << std::uppercase << (int)val

Stamp::Stamp()
{
	m_id = InvalidStampId;
	m_width = 0;
	m_height = 0;
	m_nameHash = 0;
}

Stamp::Stamp(StampId stampId, const Stamp& rhs)
{
	*this = rhs;
	m_id = stampId;
}

Stamp::Stamp(StampId stampId, int width, int height)
{
	m_id = stampId;
	m_width = width;
	m_height = height;
	m_nameHash = 0;

	int size = width * height;
	m_tiles.resize(size);

	for(int i = 0; i < size; i++)
	{
		m_tiles[i].m_id = InvalidTileId;
	}
}

StampId Stamp::GetId() const
{
	return m_id;
}

int Stamp::GetWidth() const
{
	return m_width;
}

int Stamp::GetHeight() const
{
	return m_height;
}

void Stamp::Resize(int width, int height, bool shiftRight, bool shiftDown)
{
	const int tileWidth = 8; // m_platformConfig.tileWidth;
	const int tileHeight = 8; // m_platformConfig.tileHeight;

	//Create new tile array
	std::vector<TileDesc> tiles;

	//Set new size
	int size = width * height;
	tiles.resize(size);

	//Fill with invalid tile
	TileDesc blankTile;
	blankTile.m_id = InvalidTileId;
	std::fill(tiles.begin(), tiles.end(), blankTile);

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

			tiles[destTileIdx].m_id = GetTile(x, y);
			tiles[destTileIdx].m_flags = GetTileFlags(x, y);
		}
	}

	//Set new
	m_tiles = tiles;
	m_width = width;
	m_height = height;
}

void Stamp::SetTile(int x, int y, TileId tile)
{
	int tileIdx = (y * m_width) + x;
	ion::debug::Assert(tileIdx < (m_width * m_height), "Stamp::SetTile() - Out of range");
	m_tiles[tileIdx].m_id = tile;
	m_tiles[tileIdx].m_flags = 0;
}

TileId Stamp::GetTile(int x, int y) const
{
	int tileIdx = (y * m_width) + x;
	ion::debug::Assert(tileIdx < (m_width * m_height), "Stamp::GetTile() - Out of range");
	return m_tiles[tileIdx].m_id;
}

int Stamp::GetTileIndex(TileId tileId) const
{
	for(int i = 0; i < m_tiles.size(); i++)
	{
		if(m_tiles[i].m_id == tileId)
		{
			return i;
		}
	}

	return 0;
}

void Stamp::SetTileFlags(int x, int y, u32 flags)
{
	int tileIdx = (y * m_width) + x;
	ion::debug::Assert(tileIdx < (m_width * m_height), "Stamp::SetTileFlags() - Out of range");
	m_tiles[tileIdx].m_flags = flags;
}

u32 Stamp::GetTileFlags(int x, int y) const
{
	int tileIdx = (y * m_width) + x;
	ion::debug::Assert(tileIdx < (m_width * m_height), "Stamp::GetTileFlags() - Out of range");
	return m_tiles[tileIdx].m_flags;
}

void Stamp::SetName(const std::string& name)
{
	m_name = name;
	m_nameHash = ion::Hash(name.c_str());
}

const std::string& Stamp::GetName() const
{
	return m_name;
}

u32 Stamp::GetNameHash() const
{
	return m_nameHash;
}

bool Stamp::CheckTilesBatched() const
{
	//Get all unique tiles
	std::vector<TileId> tiles;
	GetSortedUniqueTileBatch(tiles);

	//Check list is sequential
	int index = tiles.front();
	for(int i = 0; i < tiles.size(); i++, index++)
	{
		if(index != tiles[i])
		{
			return false;
		}
	}

	return true;
}

int Stamp::GetSortedUniqueTileBatch(std::vector<TileId>& tileBatch) const
{
	//Get all unique tiles
	tileBatch.reserve(GetWidth() * GetHeight());

	for(int y = 0; y < GetHeight(); y++)
	{
		for(int x = 0; x < GetWidth(); x++)
		{
			int tileIndex = GetTile(x, y);

			if(std::find(tileBatch.begin(), tileBatch.end(), tileIndex) == tileBatch.end())
			{
				tileBatch.push_back(tileIndex);
			}
		}
	}

	//Sort
	std::sort(tileBatch.begin(), tileBatch.end(), [](TileId& a, TileId& b) { return a < b; });

	return tileBatch.size();
}

TileId Stamp::GetFirstTileId() const
{
	TileId tileId = InvalidTileId;

	for(int i = 0; i < m_tiles.size(); i++)
	{
		if(m_tiles[i].m_id < tileId)
		{
			tileId = m_tiles[i].m_id;
		}
	}

	return tileId;
}

StampAnimSheetId Stamp::CreateStampAnimSheet()
{
	StampAnimSheetId stampAnimSheetId = ion::GenerateUUID64();
	m_stampAnimSheets.insert(std::make_pair(stampAnimSheetId, StampAnimSheet()));
	return stampAnimSheetId;
}

void Stamp::DeleteStampAnimSheet(StampAnimSheetId stampAnimSheetId)
{
	TStampAnimSheetMap::iterator it = m_stampAnimSheets.find(stampAnimSheetId);
	if(it != m_stampAnimSheets.end())
	{
		m_stampAnimSheets.erase(it);
	}
}

StampAnimSheet* Stamp::GetStampAnimSheet(StampAnimSheetId stampAnimSheetId)
{
	StampAnimSheet* stampAnimSheet = NULL;

	TStampAnimSheetMap::iterator it = m_stampAnimSheets.find(stampAnimSheetId);
	if(it != m_stampAnimSheets.end())
	{
		stampAnimSheet = &it->second;
	}

	return stampAnimSheet;
}

const StampAnimSheet* Stamp::GetStampAnimSheet(StampAnimSheetId stampAnimSheetId) const
{
	const StampAnimSheet* stampAnimSheet = NULL;

	TStampAnimSheetMap::const_iterator it = m_stampAnimSheets.find(stampAnimSheetId);
	if(it != m_stampAnimSheets.end())
	{
		stampAnimSheet = &it->second;
	}

	return stampAnimSheet;
}

StampAnimSheet* Stamp::FindStampAnimSheet(const std::string& name)
{
	for(TStampAnimSheetMap::iterator it = m_stampAnimSheets.begin(), end = m_stampAnimSheets.end(); it != end; ++it)
	{
		if(ion::string::CompareNoCase(it->second.GetName(), name))
		{
			return &it->second;
		}
	}

	return NULL;
}

const TStampAnimSheetMap::const_iterator Stamp::StampAnimSheetsBegin() const
{
	return m_stampAnimSheets.begin();
}

const TStampAnimSheetMap::const_iterator Stamp::StampAnimSheetsEnd() const
{
	return m_stampAnimSheets.end();
}

TStampAnimSheetMap::iterator Stamp::StampAnimSheetsBegin()
{
	return m_stampAnimSheets.begin();
}

TStampAnimSheetMap::iterator Stamp::StampAnimSheetsEnd()
{
	return m_stampAnimSheets.end();
}

int Stamp::GetStampAnimSheetCount() const
{
	return m_stampAnimSheets.size();
}

void Stamp::Serialise(ion::io::Archive& archive)
{
	archive.Serialise(m_id, "id");
	archive.Serialise(m_width, "width");
	archive.Serialise(m_height, "height");
	archive.Serialise(m_name, "name");
	archive.Serialise(m_nameHash, "nameHash");
	archive.Serialise(m_stampAnimSheets, "stampAnimSheets");
	archive.Serialise(m_tiles, "tiles");
}

void Stamp::Export(const Project& project, std::stringstream& stream) const
{
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

				const TileDesc& tileDesc = m_tiles[(y * m_width) + x];
				u8 paletteId = 0;

				//If blank tile, use background tile
				u32 tileId = (tileDesc.m_id == InvalidTileId) ? backgroundTileId : tileDesc.m_id;

				const Tile* tile = project.GetTileset().GetTile(tileId);
				ion::debug::Assert(tile, "Map::Export() - Invalid tile");

				//Generate components
				u16 tileIndex = tileId & 0x7FF;								//Bottom 11 bits = tile ID (index from 0)
				u16 flipH = (tileDesc.m_flags & Map::eFlipX) ? 1 << 11 : 0;		//12th bit = Flip X flag
				u16 flipV = (tileDesc.m_flags & Map::eFlipY) ? 1 << 12 : 0;		//13th bit = Flip Y flag
				u16 palette = (tile->GetPaletteId() & 0x3) << 13;			//14th+15th bits = Palette ID
				u16 plane = (tileDesc.m_flags & Map::eHighPlane) ? 1 << 15 : 0;	//16th bit = High plane flag

				//Generate word
				u16 word = tileIndex | flipV | flipH | palette;

				stream << "0x" << std::setw(4) << (u32)word;

				if(x < (m_width - 1))
					stream << ",";
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

void Stamp::Export(const Project& project, ion::io::File& file) const
{
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

				const TileDesc& tileDesc = m_tiles[(y * m_width) + x];
				u8 paletteId = 0;

				//If blank tile, use background tile
				u32 tileId = (tileDesc.m_id == InvalidTileId) ? backgroundTileId : tileDesc.m_id;

				const Tile* tile = project.GetTileset().GetTile(tileId);
				ion::debug::Assert(tile, "Map::Export() - Invalid tile");

				//Generate components
				u16 tileIndex = tileId & 0x7FF;								//Bottom 11 bits = tile ID (index from 0)
				u16 flipH = (tileDesc.m_flags & Map::eFlipX) ? 1 << 11 : 0;		//12th bit = Flip X flag
				u16 flipV = (tileDesc.m_flags & Map::eFlipY) ? 1 << 12 : 0;		//13th bit = Flip Y flag
				u16 palette = (tile->GetPaletteId() & 0x3) << 13;			//14th+15th bits = Palette ID
				u16 plane = (tileDesc.m_flags & Map::eHighPlane) ? 1 << 15 : 0;	//16th bit = High plane flag

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

void Stamp::ExportStampAnims(const PlatformConfig& config, std::stringstream& stream) const
{
	if(m_tiles.size() > 0 && m_stampAnimSheets.size() > 0)
	{
		u32 frameIndex = 0;

		stream << "stamp_" << m_name << ":" << std::endl << std::endl;

		//Export stamp size and tile offset
		const StampAnimSheet& stampAnimSheet = m_stampAnimSheets.begin()->second;

		std::stringstream label;
		label << "stampanimsheet_" << m_name;

		std::vector<TileId> uniqueTiles;
		int sizeUniqueTiles = GetSortedUniqueTileBatch(uniqueTiles);
		int firstUniqueTile = uniqueTiles[0];

		stream << label.str() << "_size_b\t\tequ 0x" << HEX4(sizeUniqueTiles * 32) << "\t; Size in bytes" << std::endl;
		stream << label.str() << "_size_t\t\tequ 0x" << HEX4(sizeUniqueTiles) << "\t; Size in tiles" << std::endl;
		stream << label.str() << "_tileoffset\t\tequ 0x" << HEX4(firstUniqueTile) << "\t; First tile offset" << std::endl;

		//Export frame offsets
		for(TStampAnimSheetMap::const_iterator it = m_stampAnimSheets.begin(), end = m_stampAnimSheets.end(); it != end; ++it)
		{
			std::stringstream label;
			label << "stampanimsheet_" << m_name << "_" << it->second.GetName();

			u32 size = it->second.GetBinarySizeTiles();

			stream << label.str() << "_frameoffset\tequ 0x" << HEX2(frameIndex) << "\t; Offset to first frame in stamp anim sheet" << std::endl << std::dec << std::endl << std::endl;

			frameIndex += it->second.GetNumFrames();
		}

		stream << "spritesheets_" << m_name << ":" << std::endl << std::endl;

		//Export stamp anim sheet tile data
		for(TStampAnimSheetMap::const_iterator it = m_stampAnimSheets.begin(), end = m_stampAnimSheets.end(); it != end; ++it)
		{
			std::stringstream label;
			label << "stampanimsheet_" << m_name << "_" << it->second.GetName();

			stream << label.str() << ":" << std::endl << std::endl;

			//Row major
			it->second.ExportStampTiles(config, *this, stream);

			stream << std::endl << std::endl;
		}

		stream << "stampanims_" << m_name << ":" << std::endl << std::endl;

		//Export stamp anim data
		for(TStampAnimSheetMap::const_iterator it = m_stampAnimSheets.begin(), end = m_stampAnimSheets.end(); it != end; ++it)
		{
			it->second.ExportAnims(config, stream, m_name);
			stream << std::endl;
		}
	}
}

void Stamp::ExportStampAnims(const PlatformConfig& config, ion::io::File& file) const
{

}

void Stamp::ExportStampAnimSetup(const PlatformConfig& config, std::stringstream& stream) const
{
	if(m_tiles.size() > 0 && m_stampAnimSheets.size() > 0)
	{
		//Setup first anim
		const StampAnimSheet& stampAnimSheet = m_stampAnimSheets.begin()->second;

		std::stringstream animSheetLabel;
		animSheetLabel << "stampanimsheet_" << m_name;

		std::string animationName = stampAnimSheet.AnimationsBegin()->second.GetName();

		stream << "\t; Get stamp VRAM address" << std::endl;
		stream << "\tmove.w (vram_addr_leveltiles), d0" << std::endl;
		stream << "\tadd.w  #" << animSheetLabel.str() << "_tileoffset*size_tile_b, d0" << std::endl;

		stream << "\t; Load scene anim objects" << std::endl;
		stream << "\tjsr    SceneAnimInit" << std::endl;
		stream << "\tlea    spritesheets_" << m_name << ", a2" << std::endl;
		stream << "\tmove.l #" << animSheetLabel.str() << "_size_t, d1" << std::endl;
		stream << "\tjsr    SceneAnimLoadStampAnim" << std::endl;

		stream << std::endl;

		stream << "\t; Set scene animations" << std::endl;
		stream << "\tmove.w #" << animSheetLabel.str() << "_" << animationName << "_frameoffset, Animation_FirstFrameOffset(a1)" << std::endl;
		stream << "\tmove.l #spriteanim_" << m_name << "_" << animationName << "_track_frames, Animation_AnimTrackSpriteFrame(a1)" << std::endl;
		stream << "\tmove.w #spriteanim_" << m_name << "_" << animationName << "_speed, Animation_Speed(a1)" << std::endl;
		stream << "\tmove.b #spriteanim_" << m_name << "_" << animationName << "_numframes, Animation_Length(a1)" << std::endl;
		stream << "\tmove.b #0x1, Animation_Looping(a1)" << std::endl;
		stream << "\tjsr    AnimObjSetAnimation" << std::endl;

		stream << std::endl;
		stream << "\tadd.l  #SceneAnim_Struct_Size, a0" << std::endl;
		stream << "\tadd.l  #Animation_Struct_Size, a1" << std::endl;
		stream << std::endl;
	}
}