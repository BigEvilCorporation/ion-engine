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
	m_angleByte = 0;
	m_width = defaultPlatformConfig.tileWidth;
	m_height = defaultPlatformConfig.tileHeight;
	m_heightmap.resize(m_width, 0);
	m_widthmap.resize(m_height, 0);
}

TerrainTile::TerrainTile(u8 width, u8 height)
{
	m_hash = 0;
	m_angleByte = 0;
	m_width = width;
	m_height = height;
	m_heightmap.resize(width, 0);
	m_widthmap.resize(height, 0);
}

bool TerrainTile::operator == (const TerrainTile& rhs) const
{
	return (m_heightmap == rhs.m_heightmap) && (m_widthmap == rhs.m_widthmap) && m_angleByte == rhs.m_angleByte;
}

void TerrainTile::CopyData(const TerrainTile& tile)
{
	m_heightmap = tile.m_heightmap;
	m_widthmap = tile.m_widthmap;
}

void TerrainTile::GetHeights(std::vector<s8>& heights) const
{
	heights.resize(m_width);
	ion::memory::MemCopy(heights.data(), m_heightmap.data(), m_width);
}

void TerrainTile::GetWidths(std::vector<s8>& widths) const
{
	widths.resize(m_height);
	ion::memory::MemCopy(widths.data(), m_widthmap.data(), m_height);
}

void TerrainTile::SetHeight(int x, s8 height)
{
	ion::debug::Assert(x < m_width, "TerrainTile::SetHeight() - Out of range");
	ion::debug::Assert(height >= -m_height && height <= m_height, "TerrainTile::SetHeight() - Out of range");
	m_heightmap[x] = height;
}

void TerrainTile::SetWidth(int y, s8 width)
{
	ion::debug::Assert(y < m_height, "TerrainTile::SetWidth() - Out of range");
	ion::debug::Assert(width >= -m_width && width <= m_width, "TerrainTile::SetWidth() - Out of range");
	m_widthmap[y] = width;
}

void TerrainTile::ClearHeight(int x)
{
	ion::debug::Assert(x < m_width, "TerrainTile::ClearHeight() - Out of range");
	m_heightmap[x] = 0;
}

void TerrainTile::ClearWidth(int y)
{
	ion::debug::Assert(y < m_height, "TerrainTile::ClearWidth() - Out of range");
	m_widthmap[y] = 0;
}

s8 TerrainTile::GetHeight(int x) const
{
	ion::debug::Assert(x < m_width, "TerrainTile::GetHeight() - Out of range");
	return m_heightmap[x];
}

s8 TerrainTile::GetWidth(int y) const
{
	ion::debug::Assert(y < m_height, "TerrainTile::GetWidth() - Out of range");
	return m_widthmap[y];
}

void TerrainTile::SetNormal(const ion::Vector2& normal)
{
	m_normal = normal;

	float radians = normal.Angle(ion::Vector2(0.0f, 1.0f));
	float degrees = 360.0f - ion::maths::Fmod(ion::maths::RadiansToDegrees(radians) + 360.0f, 360.0f);
	if (degrees >= 360.0f)
		degrees -= 360.0f;

	m_angleByte = (u8)ion::maths::Floor((360.0f - degrees) * (256.0f / 360.0f));
}

const ion::Vector2& TerrainTile::GetNormal() const
{
	return m_normal;
}

u8 TerrainTile::GetAngleByte() const
{
	return m_angleByte;
}

float TerrainTile::GetAngleRadians() const
{
	return m_normal.Angle(ion::Vector2(0.0f, 1.0f));
}

float TerrainTile::GetAngleDegrees() const
{
	float degrees = 360.0f - ion::maths::Fmod(ion::maths::RadiansToDegrees(GetAngleRadians()) + 360.0f, 360.0f);
	if (degrees >= 360.0f)
		degrees -= 360.0f;
	return degrees;
}

void TerrainTile::Serialise(ion::io::Archive& archive)
{
	archive.Serialise(m_width, "width");
	archive.Serialise(m_height, "height");
	archive.Serialise(m_hash, "hash");
	archive.Serialise(m_normal, "normal");
	archive.Serialise(m_angleByte, "angleByte");
	archive.Serialise(m_heightmap, "heightMap");
	archive.Serialise(m_widthmap, "widthMap");
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
	std::vector<u8> data;
	data.reserve((m_width * m_height) + 1);
	data.insert(data.end(), m_heightmap.begin(), m_heightmap.end());
	data.insert(data.end(), m_widthmap.begin(), m_widthmap.end());
	data.push_back(m_angleByte);

	m_hash = ion::Hash64((const u8*)data.data(), data.size());
	return m_hash;
}

u64 TerrainTile::GetHash() const
{
	return m_hash;
}
