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
#include <ion/maths/Vector.h>
#include <sstream>

enum eCollisionTileFlags
{
	eCollisionTileFlagTerrain = (1 << 11),
	eCollisionTileFlagWater = (1 << 12),
	eCollisionTileFlagSolid = (1 << 13),
	eCollisionTileFlagHole = (1 << 14),
	eCollisionTileFlagSpecial = (1 << 15),

	eCollisionTileFlagAll = (eCollisionTileFlagTerrain | eCollisionTileFlagWater | eCollisionTileFlagSolid | eCollisionTileFlagHole | eCollisionTileFlagSpecial),
	eCollisionTileFlagNone = (eCollisionTileFlagTerrain - 1)
};

typedef u32 TerrainTileId;
static const TerrainTileId InvalidTerrainTileId = eCollisionTileFlagNone;

class TerrainTile
{
public:
	TerrainTile();
	TerrainTile(u8 width, u8 height);

	bool operator == (const TerrainTile& rhs) const;

	u64 CalculateHash();
	u64 GetHash() const;

	void SetHeight(int x, s8 height);
	void SetWidth(int y, s8 width);

	void ClearHeight(int x);
	void ClearWidth(int y);

	s8 GetHeight(int x) const;
	s8 GetWidth(int y) const;

	void GetHeights(std::vector<s8>& heights) const;
	void GetWidths(std::vector<s8>& widths) const;

	void SetNormal(const ion::Vector2& normal);
	const ion::Vector2& GetNormal() const;
	u8 GetAngleByte() const;
	float GetAngleRadians() const;
	float GetAngleDegrees() const;

	void CopyData(const TerrainTile& tile);

	void Serialise(ion::io::Archive& archive);
	void Export(std::stringstream& stream) const;
	void Export(ion::io::File& file) const;

private:
	u64 m_hash;
	u8 m_width;
	u8 m_height;
	u8 m_angleByte;
	ion::Vector2 m_normal;
	std::vector<s8> m_heightmap;
	std::vector<s8> m_widthmap;
};