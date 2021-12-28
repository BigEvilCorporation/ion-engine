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

#include "TerrainTile.h"
#include "TerrainTileset.h"

typedef u32 TerrainStampId;
static const TerrainStampId InvalidTerrainStampId = 0;

class Project;

class TerrainStamp
{
public:
	TerrainStamp();
	TerrainStamp(TerrainStampId TerrainStampId, const TerrainStamp& rhs);
	TerrainStamp(TerrainStampId TerrainStampId, int width, int height);

	TerrainStampId GetId() const;
	int GetWidth() const;
	int GetHeight() const;
	int GetNumLayers() const;

	void SetNumLayers(int numLayers);
	void Resize(int w, int h, bool shiftRight, bool shiftDown);

	void SetTile(int x, int y, TerrainTileId tile, int layer = 0);
	TerrainTileId GetTile(int x, int y, int layer = 0) const;
	int GetTileIndex(TerrainTileId TerrainTileId, int layer = 0) const;

	void SetTileFlags(int x, int y, u32 flags, int layer = 0);
	u32 GetTileFlags(int x, int y, int layer = 0) const;

	void Serialise(ion::io::Archive& archive);

private:
	TerrainStampId m_id;
	int m_width;
	int m_height;

	struct TileDesc
	{
		TileDesc() { m_id = 0; m_flags = 0; }

		void Serialise(ion::io::Archive& archive)
		{
			archive.Serialise(m_id);
			archive.Serialise(m_flags);
		}

		TerrainTileId m_id;
		u32 m_flags;
	};

	std::vector<std::vector<TileDesc>> m_layers;
};
