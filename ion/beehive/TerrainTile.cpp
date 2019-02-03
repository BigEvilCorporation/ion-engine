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

#include <core/debug/Debug.h>
#include <core/memory/Memory.h>
#include <core/cryptography/Hash.h>
#include <maths/Vector.h>

#include <string>
#include <iomanip>

#include "TerrainTile.h"
#include "PlatformConfig.h"

TerrainTile::TerrainTile()
{
	const PlatformConfig& defaultPlatformConfig = PlatformPresets::s_configs[PlatformPresets::ePresetMegaDrive];

	m_hash = 0;
	m_width = defaultPlatformConfig.tileWidth;
	m_height = defaultPlatformConfig.tileHeight;
}

TerrainTile::TerrainTile(u8 width, u8 height)
{
	m_hash = 0;
	m_width = width;
	m_height = height;
	m_heightmap.resize(width * height, 0);
}

bool TerrainTile::operator == (const TerrainTile& rhs) const
{
	return m_heightmap == rhs.m_heightmap;
}

float TerrainTile::CalculateAngle() const
{
	ion::Vector2 point1;
	ion::Vector2 point2;

	int leftY = m_heightmap[0];
	int rightY = m_heightmap[m_width - 1];

	//Find left point - X coord at which height first changes, scanning left-to-right
	for (int x = 1; x < m_width; x++)
	{
		if (m_heightmap[x] != leftY)
		{
			point1.x = (float)x - 1;
			point1.y = ion::maths::Min((float)m_heightmap[x], (float)leftY);
			break;
		}
	}

	//Find right point - X coord at which height first changes, scanning right-to-left
	for (int x = m_width - 1; x >= 0; x--)
	{
		if (m_heightmap[x] != rightY)
		{
			point2.x = (float)x;
			point2.y = ion::maths::Max((float)m_heightmap[x], (float)rightY);
			break;
		}
	}

	//Calculate angle
	float angle = 0;

	if (point1.y >= 0.0f && point2.y >= 0.0f)
	{
		//If at same X coord, invert
		if (point1.x == point2.x)
		{
			point2.y = -point2.y;
		}

		ion::Vector2 vector = point2 - point1;
		angle = ion::Vector2(1.0f, 0.0f).Angle(vector);
	}

	return angle;
}

void TerrainTile::CopyHeights(const TerrainTile& tile)
{
	m_heightmap = tile.m_heightmap;
}

void TerrainTile::GetHeights(std::vector<s8>& heights) const
{
	heights.resize(m_heightmap.size());
	ion::memory::MemCopy(heights.data(), m_heightmap.data(), m_heightmap.size());
}

void TerrainTile::SetHeight(int x, s8 height)
{
	ion::debug::Assert(x < m_width, "TerrainTile::SetHeight() - Out of range");
	ion::debug::Assert(height >= -m_height && height <= m_height, "TerrainTile::SetHeight() - Out of range");
	m_heightmap[x] = height;
}

void TerrainTile::ClearHeight(int x)
{
	ion::debug::Assert(x < m_width, "TerrainTile::ClearHeight() - Out of range");
	m_heightmap[x] = 0;
}

s8 TerrainTile::GetHeight(int x) const
{
	ion::debug::Assert(x < m_width, "TerrainTile::GetHeight() - Out of range");
	return m_heightmap[x];
}

void TerrainTile::Serialise(ion::io::Archive& archive)
{
	archive.Serialise(m_width, "width");
	archive.Serialise(m_height, "height");
	archive.Serialise(m_hash, "hash");
	archive.Serialise(m_heightmap, "heightMap");
}

void TerrainTile::Export(std::stringstream& stream) const
{
	stream << std::hex << std::setfill('0') << std::uppercase;

	stream << "\tdc.b\t";

	//1 byte per height
	for(int x = 0; x < m_width; x++)
	{
		stream << "0x";
		stream << std::setw(1) << (int)GetHeight(x);

		if(x < (m_width-1))
			stream << ", ";
	}

	stream << std::dec;
}

void TerrainTile::Export(ion::io::File& file) const
{
	//1 byte per width
	for(int x = 0; x < m_width; x++)
	{
		s8 byte = GetHeight(x);
		file.Write(&byte, sizeof(s8));
	}
}

u64 TerrainTile::CalculateHash()
{
	m_hash = ion::Hash64((const u8*)m_heightmap.data(), m_heightmap.size());
	return m_hash;
}

u64 TerrainTile::GetHash() const
{
	return m_hash;
}
