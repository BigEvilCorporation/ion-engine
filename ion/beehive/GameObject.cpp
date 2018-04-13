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

#include "GameObject.h"
#include <algorithm>
#include <iomanip>

GameObjectType::GameObjectType()
{
	m_id = InvalidGameObjectTypeId;
	m_previewSpriteSheetId = InvalidSpriteSheetId;
	m_initPriority = -1;
}

GameObjectType::GameObjectType(u32 id)
{
	m_id = id;
	m_name = "GameObj_";
	m_previewSpriteSheetId = InvalidSpriteSheetId;
	m_dimensions.x = 16;
	m_dimensions.y = 16;
	m_initPriority = m_id;
}

bool GameObjectType::LoadPreviewSprite(const std::string& filename)
{
	if(m_previewSpriteSheetId == InvalidSpriteSheetId)
	{
		m_previewSpriteSheetId = ion::GenerateUUID64();
	}

	return m_previewSpriteSheet.ImportBitmap(filename, m_name, 8, 8, 1, 1, 1);
}

GameObjectVariable& GameObjectType::AddVariable()
{
	m_variables.push_back(GameObjectVariable());
	return m_variables[m_variables.size() - 1];
}

void GameObjectType::RemoveVariable(GameObjectVariable& variable)
{
	for(int i = 0; i < m_variables.size(); i++)
	{
		if(m_variables[i].m_name == variable.m_name)
		{
			std::swap(variable, m_variables.back());
			m_variables.pop_back();
			return;
		}
	}
}

GameObjectVariable* GameObjectType::GetVariable(u32 index)
{
	ion::debug::Assert(index < m_variables.size(), "GameObjectType::GetVariable() - out of range");
	return &m_variables[index];
}

void GameObjectType::Serialise(ion::io::Archive& archive)
{
	archive.Serialise(m_id, "id");
	archive.Serialise(m_name, "name");
	archive.Serialise(m_spriteActorId, "spriteActorId");
	archive.Serialise(m_previewSpriteSheetId, "previewSpriteSheetId");
	archive.Serialise(m_previewSpriteSheet, "previewSpriteSheetData");
	archive.Serialise(m_variables, "variables");
	archive.Serialise(m_initPriority, "initPriority");
	archive.Serialise(m_dimensions, "dimensions");

	if(m_initPriority < 0)
	{
		m_initPriority = m_id;
	}
}

GameObject::GameObject()
{
	m_objectId = InvalidGameObjectId;
	m_typeId = InvalidGameObjectTypeId;
	m_spriteSheetId = InvalidSpriteSheetId;
	m_spriteAnimId = InvalidSpriteAnimId;
}

GameObject::GameObject(GameObjectId objectId, const GameObject& rhs)
{
	m_objectId = objectId;
	m_typeId = rhs.m_typeId;
	m_position = rhs.m_position;
	m_name = rhs.m_name;
	m_spriteSheetId = rhs.m_spriteSheetId;
	m_spriteAnimId = rhs.m_spriteAnimId;
	m_variables = rhs.m_variables;
	m_dimensions = rhs.m_dimensions;
}

GameObject::GameObject(GameObjectId objectId, GameObjectTypeId typeId, const ion::Vector2i& position)
{
	m_objectId = objectId;
	m_typeId = typeId;
	m_position = position;

	m_spriteSheetId = InvalidSpriteSheetId;
	m_spriteAnimId = InvalidSpriteAnimId;
}

GameObject::GameObject(GameObjectId objectId, GameObjectTypeId typeId, const ion::Vector2i& position, const ion::Vector2i& dimensions)
{
	m_objectId = objectId;
	m_typeId = typeId;
	m_position = position;
	m_dimensions = dimensions;

	m_spriteSheetId = InvalidSpriteSheetId;
	m_spriteAnimId = InvalidSpriteAnimId;
}

GameObjectVariable& GameObject::AddVariable()
{
	m_variables.push_back(GameObjectVariable());
	return m_variables[m_variables.size() - 1];
}

void GameObject::RemoveVariable(GameObjectVariable& variable)
{
	for(int i = 0; i < m_variables.size(); i++)
	{
		if(m_variables[i].m_name == variable.m_name)
		{
			std::swap(variable, m_variables.back());
			m_variables.pop_back();
			return;
		}
	}
}

GameObjectVariable* GameObject::GetVariable(u32 index)
{
	ion::debug::Assert(index < m_variables.size(), "GameObject::GetVariable() - out of range");
	return &m_variables[index];
}

void GameObject::Serialise(ion::io::Archive& archive)
{
	archive.Serialise(m_objectId, "objectId");
	archive.Serialise(m_typeId, "typeId");
	archive.Serialise(m_name, "name");
	archive.Serialise(m_position, "position");
	archive.Serialise(m_dimensions, "dimensions");
	archive.Serialise(m_variables, "variables");
}

void GameObject::Export(std::stringstream& stream, const GameObjectType& gameObjectType, const std::string& name, int mapWidth) const
{
	//Initialise object
	stream << '\t' << "jsr " << gameObjectType.GetName() << "Init" << std::endl;

	const std::vector<GameObjectVariable>& templateVariables = gameObjectType.GetVariables();

	for(int i = 0; i < templateVariables.size(); i++)
	{
		//"move.[s] #[value], [name](a0)"

		std::string valueString = templateVariables[i].m_value;
		bool overrideLine = ParseValueTokens(valueString, templateVariables[i].m_name, (GameObjectVariableSize)templateVariables[i].m_size);

		if(overrideLine)
		{
			stream << valueString;
		}
		else
		{
			stream << '\t';

			switch(templateVariables[i].m_size)
			{
			case eSizeByte:
				stream << "move.b ";
				break;
			case eSizeWord:
				stream << "move.w ";
				break;
			case eSizeLong:
				stream << "move.l ";
				break;
			}

			stream << "#" << valueString << ", " << templateVariables[i].m_name << "(a0)" << std::endl;
		}
	}

	const std::vector<GameObjectVariable>& instanceVariables = GetVariables();

	for(int i = 0; i < instanceVariables.size(); i++)
	{
		//"move.[s] #[value], [name](a0)"

		std::string valueString = instanceVariables[i].m_value;
		bool overrideLine = ParseValueTokens(valueString, instanceVariables[i].m_name, (GameObjectVariableSize)instanceVariables[i].m_size);

		if(overrideLine)
		{
			stream << valueString << std::endl;
		}
		else
		{
			stream << '\t';

			switch(instanceVariables[i].m_size)
			{
			case eSizeByte:
				stream << "move.b ";
				break;
			case eSizeWord:
				stream << "move.w ";
				break;
			case eSizeLong:
				stream << "move.l ";
				break;
			}

			stream << "#" << valueString << ", " << instanceVariables[i].m_name << "(a0)" << std::endl;
		}
	}

	//Add to quadtree
#if GAMEOBJ_EXPORT_QUADTREE
	const int quadTreeWidth = 320;
	const int quadTreeHeight = 224;
	const int cellPosX = m_position.x / quadTreeWidth;
	const int cellPosY = m_position.y / quadTreeHeight;
	const int worldWidthCells = mapWidth / quadTreeWidth;
	const int quadTreeCell = (cellPosY * worldWidthCells) + cellPosX;
	stream << "\tmove.w #0x" << std::hex << std::setfill('0') << std::setw(4) << quadTreeCell << ", d0" << std::endl;
	stream << "\tjsr EntityAddToQuadTree" << std::endl;
#endif

	//Load graphics
	stream << '\t' << "jsr " << gameObjectType.GetName() << "LoadGfx" << std::endl;

	//Copy debug name
	static const int maxDebugNameSize = 16;
	stream << '\t' << "IFND FINAL" << std::endl;
	stream << '\t' << "lea " << name << "_name, a3" << std::endl;
	stream << '\t' << "move.l a0, a2" << std::endl;
	stream << '\t' << "add.l #Entity_DebugName, a2" << std::endl;
	stream << '\t' << "move.l #0x" << ion::maths::Min((int)name.size(), maxDebugNameSize) << ", d0" << std::endl;
	stream << '\t' << "MEMCPYB a2,a3,d0" << std::endl;
	stream << '\t' << "ENDIF" << std::endl;
}

bool GameObject::ParseValueTokens(std::string& valueString, const std::string& varName, GameObjectVariableSize size) const
{
	const std::string worldPosXString("WORLDPOSX");
	const std::string worldPosYString("WORLDPOSY");
	const std::string widthString("WIDTH");
	const std::string heightString("HEIGHT");
	const std::string ntscString("NTSC(");

	std::string::size_type tokenStart = valueString.find("&");

	if(tokenStart != std::string::npos)
	{
		// "&WORLDPOSX"
		std::string::size_type worldPosXStart = valueString.find(worldPosXString, tokenStart);
		if(worldPosXStart != std::string::npos)
		{
			u32 worldPosX = (m_position.x + spriteSheetBorderX) << screenToWorldSpaceShift;

			std::stringstream hexStream;
			hexStream << "0x" << std::hex << std::setfill('0') << std::setw(size * 2) << worldPosX;

			valueString.replace(worldPosXStart - 1, worldPosXString.size() + 1, hexStream.str());

			return false;
		}

		// "&WORLDPOSY"
		std::string::size_type worldPosYStart = valueString.find(worldPosYString);
		if(worldPosYStart != std::string::npos)
		{
			u32 worldPosY = (m_position.y + spriteSheetBorderY) << screenToWorldSpaceShift;

			std::stringstream hexStream;
			hexStream << "0x" << std::hex << std::setfill('0') << std::setw(size * 2) << worldPosY;

			valueString.replace(worldPosYStart - 1, worldPosYString.size() + 1, hexStream.str());

			return false;
		}

		// "&WIDTH"
		std::string::size_type widthStart = valueString.find(widthString, tokenStart);
		if(widthStart != std::string::npos)
		{
			u32 width = m_dimensions.x;

			if(width > 0)
			{
				std::stringstream hexStream;
				hexStream << "0x" << std::hex << std::setfill('0') << std::setw(size * 2) << width;

				valueString.replace(widthStart - 1, widthString.size() + 1, hexStream.str());

				return false;
			}
			else
			{
				valueString = "";
				return true;
			}
		}

		// "&HEIGHT"
		std::string::size_type heightStart = valueString.find(heightString, tokenStart);
		if(heightStart != std::string::npos)
		{
			u32 height = m_dimensions.y;

			if(height > 0)
			{
				std::stringstream hexStream;
				hexStream << "0x" << std::hex << std::setfill('0') << std::setw(size * 2) << height;

				valueString.replace(heightStart - 1, heightString.size() + 1, hexStream.str());

				return false;
			}
			else
			{
				valueString = "";
				return true;
			}
		}

		// "&NTSC(value)"
		std::string::size_type ntscStart = valueString.find(ntscString);
		if(ntscStart != std::string::npos)
		{
			std::string::size_type valueStart = ntscStart + ntscString.size();
			std::string::size_type valueEnd = valueString.size() - 1;
			std::string ntscValue = valueString.substr(valueStart, (valueEnd - valueStart));

			std::stringstream stream;

			if(size == eSizeByte)
			{
				stream << "\tMOVE_NTSC_B " << ntscValue << ", " << varName << "(a0), d0" << std::endl;
			}
			else if(size == eSizeWord)
			{
				stream << "\tMOVE_NTSC_W " << ntscValue << ", " << varName << "(a0), d0" << std::endl;
			}
			else if(size == eSizeLong)
			{
				stream << "\tMOVE_NTSC_L " << ntscValue << ", " << varName << "(a0), d0" << std::endl;
			}

			valueString = stream.str();

			return true;
		}
	}

	return false;
}
