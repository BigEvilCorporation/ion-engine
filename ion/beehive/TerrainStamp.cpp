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

#include "TerrainStamp.h"
#include "Project.h"

#include <core/memory/Memory.h>
#include <core/memory/Endian.h>
#include <core/string/String.h>
#include <core/cryptography/Hash.h>

TerrainStamp::TerrainStamp()
{
	m_id = InvalidTerrainStampId;
	m_width = 0;
	m_height = 0;
}

TerrainStamp::TerrainStamp(TerrainStampId TerrainStampId, const TerrainStamp& rhs)
{
	*this = rhs;
	m_id = TerrainStampId;
}

TerrainStamp::TerrainStamp(TerrainStampId TerrainStampId, int width, int height)
{
	m_id = TerrainStampId;
	m_width = width;
	m_height = height;

	int size = width * height;
	m_layers.resize(1);
	m_layers[0].resize(size);

	for(int i = 0; i < size; i++)
	{
		m_layers[0][i].m_id = InvalidTerrainTileId;
	}
}

TerrainStampId TerrainStamp::GetId() const
{
	return m_id;
}

int TerrainStamp::GetWidth() const
{
	return m_width;
}

int TerrainStamp::GetHeight() const
{
	return m_height;
}

int TerrainStamp::GetNumLayers() const
{
	return m_layers.size();
}

void TerrainStamp::SetNumLayers(int numLayers)
{
	m_layers.resize(numLayers);
}

void TerrainStamp::Resize(int width, int height, bool shiftRight, bool shiftDown)
{
	const int tileWidth = 8; // m_platformConfig.tileWidth;
	const int tileHeight = 8; // m_platformConfig.tileHeight;

	//Create new tile array
	std::vector<TileDesc> tiles;

	//Set new size
	int size = width * height;
	tiles.resize(size);

	for (int layer = 0; layer < m_layers.size(); layer++)
	{
		//Fill with invalid tile
		TileDesc blankTile;
		blankTile.m_id = InvalidTerrainTileId;
		std::fill(tiles.begin(), tiles.end(), blankTile);

		//Copy tiles
		for (int x = 0; x < ion::maths::Min(width, m_width); x++)
		{
			for (int y = 0; y < ion::maths::Min(height, m_height); y++)
			{
				int destTerrainTileIdx = (y * width) + x;
				if (shiftRight && width > m_width)
					destTerrainTileIdx += (width - m_width);
				if (shiftDown && height > m_height)
					destTerrainTileIdx += (height - m_height) * width;

				tiles[destTerrainTileIdx].m_id = GetTile(x, y, layer);
				tiles[destTerrainTileIdx].m_flags = GetTileFlags(x, y, layer);
			}
		}

		//Set new
		m_layers[layer] = tiles;
	}

	m_width = width;
	m_height = height;
}

void TerrainStamp::SetTile(int x, int y, TerrainTileId tile, int layer)
{
	int terrainTileIdx = (y * m_width) + x;
	ion::debug::Assert(layer < m_layers.size(), "TerrainStamp::SetTile() - Layer out of range");
	ion::debug::Assert(terrainTileIdx < (m_width * m_height), "TerrainStamp::SetTile() - Tile index out of range");
	m_layers[layer][terrainTileIdx].m_id = tile;
	m_layers[layer][terrainTileIdx].m_flags = 0;
}

TerrainTileId TerrainStamp::GetTile(int x, int y, int layer) const
{
	int terrainTileIdx = (y * m_width) + x;
	ion::debug::Assert(layer < m_layers.size(), "TerrainStamp::GetTile() - Layer out of range");
	ion::debug::Assert(terrainTileIdx < (m_width * m_height), "TerrainStamp::GetTile() - Tile index out of range");
	return m_layers[layer][terrainTileIdx].m_id;
}

int TerrainStamp::GetTileIndex(TerrainTileId terrainTileId, int layer) const
{
	for(int i = 0; i < m_layers[layer].size(); i++)
	{
		if(m_layers[layer][i].m_id == terrainTileId)
		{
			return i;
		}
	}

	return 0;
}

void TerrainStamp::SetTileFlags(int x, int y, u32 flags, int layer)
{
	int terrainTileIdx = (y * m_width) + x;
	ion::debug::Assert(layer < m_layers.size(), "TerrainStamp::SetTileFlags() - Layer out of range");
	ion::debug::Assert(terrainTileIdx < (m_width * m_height), "TerrainStamp::SetTileFlags() - Tile index out of range");
	m_layers[layer][terrainTileIdx].m_flags = flags;
}

u32 TerrainStamp::GetTileFlags(int x, int y, int layer) const
{
	int terrainTileIdx = (y * m_width) + x;
	ion::debug::Assert(layer < m_layers.size(), "TerrainStamp::GetTileFlags() - Layer out of range");
	ion::debug::Assert(terrainTileIdx < (m_width * m_height), "TerrainStamp::GetTileFlags() - Tile index out of range");
	return m_layers[layer][terrainTileIdx].m_flags;
}

void TerrainStamp::Serialise(ion::io::Archive& archive)
{
	archive.Serialise(m_id, "id");
	archive.Serialise(m_width, "width");
	archive.Serialise(m_height, "height");
	archive.Serialise(m_layers, "layers");

	//Legacy single layer
	if (archive.GetDirection() == ion::io::Archive::Direction::In && m_layers.size() == 0)
	{
		m_layers.resize(1);
		archive.Serialise(m_layers[0], "tiles");
	}
}
