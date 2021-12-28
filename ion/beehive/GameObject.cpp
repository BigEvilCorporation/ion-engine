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
#include <vector>

#include <ion/core/string/String.h>
#include <ion/core/utils/STL.h>

const std::vector<GameObject::AutoVar> GameObject::s_autoVars
(
	{
		{ "World Position X",	"WORLDPOSX",	"Entity_WorldPosX",	eSizeLong,	[](const GameObject& gameObject, const GameObjectType& gameObjectType) { return (gameObject.GetPosition().x + spriteSheetBorderX) << screenToWorldSpaceShift; }, true },
		{ "World Position Y",	"WORLDPOSY",	"Entity_WorldPosY",	eSizeLong,	[](const GameObject& gameObject, const GameObjectType& gameObjectType) { return (gameObject.GetPosition().y + spriteSheetBorderY) << screenToWorldSpaceShift; }, true },
//TODO: Define for TW legacy mode
#if defined BEEHIVE_PLUGIN_LUMINARY
		{ "Width",				"WIDTH",		"Entity_Width",		eSizeWord,	[](const GameObject& gameObject, const GameObjectType& gameObjectType) { return gameObject.GetDimensions().x ? gameObject.GetDimensions().x : gameObjectType.GetDimensions().x; }, false },
		{ "Height",				"HEIGHT",		"Entity_Height",	eSizeWord,	[](const GameObject& gameObject, const GameObjectType& gameObjectType) { return gameObject.GetDimensions().y ? gameObject.GetDimensions().y : gameObjectType.GetDimensions().y; }, false },
#else
		{ "Width",				"WIDTH",		"Entity_Width",		eSizeWord,	[](const GameObject& gameObject, const GameObjectType& gameObjectType) { return gameObject.GetDimensions().x; }, false },
		{ "Height",				"HEIGHT",		"Entity_Height",	eSizeWord,	[](const GameObject& gameObject, const GameObjectType& gameObjectType) { return gameObject.GetDimensions().y; }, false },
#endif
	}
);

const GameObjectVariable* GameObjectType::PrefabChild::FindVariableByTag(const std::string& tag, int componentIdx) const
{
	for (int i = 0; i < variables.size(); i++)
	{
		if (((componentIdx == -1) || (componentIdx == variables[i].m_componentIdx)) && variables[i].HasTag(tag))
		{
			return &variables[i];
		}
	}

	return nullptr;
}

GameObjectType::GameObjectType()
{
	m_id = InvalidGameObjectTypeId;
	m_previewSpriteSheetId = InvalidSpriteSheetId;
	m_initPriority = -1;
	m_static = false;
	m_prefab = false;
}

GameObjectType::GameObjectType(u32 id)
{
	m_id = id;
	m_name = "GameObj_";
	m_previewSpriteSheetId = InvalidSpriteSheetId;
	m_dimensions.x = 16;
	m_dimensions.y = 16;
	m_static = false;
	m_prefab = false;
	m_initPriority = m_id;
}

bool GameObjectType::LoadPreviewSprite(const std::string& filename)
{
	if(m_previewSpriteSheetId == InvalidSpriteSheetId)
	{
		m_previewSpriteSheetId = ion::GenerateUUID64();
	}

	return m_previewSpriteSheet.ImportBitmap(filename, m_name, 8, 8, 1, 1, 0, 0);
}

void GameObjectType::AddPrefabChild(const GameObjectType::PrefabChild& child)
{
	m_prefabChildren.push_back(child);
}

void GameObjectType::RemovePrefabChild(GameObjectType::PrefabChild& child)
{
	std::vector<PrefabChild>::iterator it = std::find_if(m_prefabChildren.begin(), m_prefabChildren.end(), [&](const PrefabChild& rhs) { return rhs.typeId == child.typeId && rhs.instanceId == child.instanceId; });
	if (it != m_prefabChildren.end())
	{
		m_prefabChildren.erase(it);
	}
}

GameObjectType::PrefabChild* GameObjectType::FindPrefabChild(GameObjectId instanceId)
{
	std::vector<PrefabChild>::iterator it = std::find_if(m_prefabChildren.begin(), m_prefabChildren.end(), [&](const PrefabChild& rhs) { return rhs.instanceId == instanceId; });
	if (it != m_prefabChildren.end())
	{
		return &(*it);
	}

	return nullptr;
}

const std::vector<GameObjectType::PrefabChild>& GameObjectType::GetPrefabChildren() const
{
	return m_prefabChildren;
}

std::vector<GameObjectType::PrefabChild>& GameObjectType::GetPrefabChildren()
{
	return m_prefabChildren;
}

const GameObjectArchetype* GameObjectType::GetArchetype(GameObjectArchetypeId id) const
{
	const GameObjectArchetype* archetype = nullptr;
	TGameObjectArchetypeMap::const_iterator it = m_archetypes.find(id);
	if (it != m_archetypes.end())
		archetype = &it->second;
	return archetype;
}

GameObjectArchetype* GameObjectType::GetArchetype(GameObjectArchetypeId id)
{
	GameObjectArchetype* archetype = nullptr;
	TGameObjectArchetypeMap::iterator it = m_archetypes.find(id);
	if (it != m_archetypes.end())
		archetype = &it->second;
	return archetype;
}

const GameObjectArchetype* GameObjectType::GetArchetype(const std::string& name) const
{
	for (TGameObjectArchetypeMap::const_iterator it = m_archetypes.begin(), end = m_archetypes.end(); it != end; ++it)
	{
		if (it->second.name == name)
		{
			return &it->second;
		}
	}

	return nullptr;
}

GameObjectArchetypeId GameObjectType::AddArchetype(GameObjectArchetype& archetype)
{
	ion::debug::Assert(archetype.typeId == GetId(), "GameObjectType::AddArchetype() - Archetype does not match this game object type");
	archetype.id = ion::GenerateUUID64();
	m_archetypes.insert(std::make_pair(archetype.id, archetype));
	return archetype.id;
}

GameObjectArchetypeId GameObjectType::CreateArchetypeFromState(const GameObject& gameObject)
{
	ion::debug::Assert(gameObject.GetTypeId() == GetId(), "GameObjectType::CreateArchetypeFromState() - Game object does not match this game object type");

	GameObjectArchetype archetype;

	archetype.name = gameObject.GetName() + "_archetype";
	archetype.typeId = GetId();
	archetype.variables = gameObject.GetVariables();
	archetype.spriteActorId = gameObject.GetSpriteActorId();
	archetype.spriteSheetId = gameObject.GetSpriteSheetId();
	archetype.spriteAnimId = gameObject.GetSpriteAnim();

	return AddArchetype(archetype);
}

const TGameObjectArchetypeMap& GameObjectType::GetArchetypes() const
{
	return m_archetypes;
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

void GameObjectType::ClearVariables()
{
	m_variables.clear();
}

GameObjectVariable* GameObjectType::GetVariable(u32 index)
{
	ion::debug::Assert(index < m_variables.size(), "GameObjectType::GetVariable() - out of range");
	return &m_variables[index];
}

const GameObjectVariable* GameObjectType::GetVariable(u32 index) const
{
	ion::debug::Assert(index < m_variables.size(), "GameObjectType::GetVariable() - out of range");
	return &m_variables[index];
}

GameObjectVariable* GameObjectType::FindVariable(const std::string& name, int componentIdx)
{
	for (int i = 0; i < m_variables.size(); i++)
	{
		if (((componentIdx == -1) || (componentIdx == m_variables[i].m_componentIdx)) && ion::string::CompareNoCase(m_variables[i].m_name, name))
		{
			return &m_variables[i];
		}
	}

	return nullptr;
}

const GameObjectVariable* GameObjectType::FindVariable(const std::string& name, int componentIdx) const
{
	for (int i = 0; i < m_variables.size(); i++)
	{
		if (((componentIdx == -1) || (componentIdx == m_variables[i].m_componentIdx)) && ion::string::CompareNoCase(m_variables[i].m_name, name))
		{
			return &m_variables[i];
		}
	}

	return nullptr;
}

const GameObjectVariable* GameObjectType::FindVariableByTag(const std::string& tag, int componentIdx) const
{
	for (int i = 0; i < m_variables.size(); i++)
	{
		if (((componentIdx == -1) || (componentIdx == m_variables[i].m_componentIdx)) && m_variables[i].HasTag(tag))
		{
			return &m_variables[i];
		}
	}

	return nullptr;
}

void GameObjectType::Serialise(ion::io::Archive& archive)
{
	archive.Serialise(m_id, "id");
	archive.Serialise(m_name, "name");
	archive.Serialise(m_prefabName, "prefabName");
	archive.Serialise(m_variables, "variables");
	archive.Serialise(m_scriptVariables, "scriptVariables");
	archive.Serialise(m_scriptFunctions, "scriptFunctions");
	archive.Serialise(m_prefabChildren, "prefabChildren");
	archive.Serialise(m_archetypes, "archetypes");
	archive.Serialise(m_initPriority, "initPriority");
	archive.Serialise(m_dimensions, "dimensions");
	archive.Serialise(m_static, "static");
	archive.Serialise(m_prefab, "prefab");

	if (archive.GetContentType() == ion::io::Archive::Content::Full)
	{
		archive.Serialise(m_spriteActorId, "spriteActorId");
		archive.Serialise(m_spriteSheetId, "spriteSheetId");
		archive.Serialise(m_spriteAnimId, "spriteAnimId");
		archive.Serialise(m_previewSpriteSheetId, "previewSpriteSheetId");
		archive.Serialise(m_previewSpriteSheet, "previewSpriteSheetData");
	}

	if(m_initPriority < 0)
	{
		m_initPriority = m_id;
	}
}

GameObject::GameObject()
{
	m_objectId = InvalidGameObjectId;
	m_typeId = InvalidGameObjectTypeId;
	m_spriteActorId = InvalidActorId;
	m_spriteSheetId = InvalidSpriteSheetId;
	m_spriteAnimId = InvalidSpriteAnimId;
	m_originalArchetypeId = InvalidGameObjectArchetypeId;
}

GameObject::GameObject(GameObjectId objectId, const GameObject& rhs)
{
	m_objectId = objectId;
	m_typeId = rhs.m_typeId;
	m_position = rhs.m_position;
	m_name = rhs.m_name;
	m_spriteActorId = rhs.m_spriteActorId;
	m_spriteSheetId = rhs.m_spriteSheetId;
	m_spriteAnimId = rhs.m_spriteAnimId;
	m_variables = rhs.m_variables;
	m_dimensions = rhs.m_dimensions;
	m_originalArchetypeId = InvalidGameObjectArchetypeId;
}

GameObject::GameObject(GameObjectId objectId, GameObjectTypeId typeId, const GameObject& rhs)
{
	m_objectId = objectId;
	m_typeId = typeId;
	m_position = rhs.m_position;
	m_name = rhs.m_name;
	m_spriteActorId = rhs.m_spriteActorId;
	m_spriteSheetId = rhs.m_spriteSheetId;
	m_spriteAnimId = rhs.m_spriteAnimId;
	m_variables = rhs.m_variables;
	m_dimensions = rhs.m_dimensions;
	m_originalArchetypeId = InvalidGameObjectArchetypeId;
}

GameObject::GameObject(GameObjectId objectId, GameObjectTypeId typeId, const ion::Vector2i& position)
{
	m_objectId = objectId;
	m_typeId = typeId;
	m_position = position;

	m_spriteSheetId = InvalidSpriteSheetId;
	m_spriteAnimId = InvalidSpriteAnimId;
	m_originalArchetypeId = InvalidGameObjectArchetypeId;
}

GameObject::GameObject(GameObjectId objectId, GameObjectTypeId typeId, const ion::Vector2i& position, const ion::Vector2i& dimensions)
{
	m_objectId = objectId;
	m_typeId = typeId;
	m_position = position;
	m_dimensions = dimensions;

	m_spriteSheetId = InvalidSpriteSheetId;
	m_spriteAnimId = InvalidSpriteAnimId;
	m_originalArchetypeId = InvalidGameObjectArchetypeId;
}

void GameObject::ApplyArchetype(const GameObjectArchetype& archetype)
{
	m_originalArchetypeId = archetype.id;
	m_typeId = archetype.typeId;
	m_variables = archetype.variables;
	m_spriteActorId = archetype.spriteActorId;
	m_spriteSheetId = archetype.spriteSheetId;
	m_spriteAnimId = archetype.spriteAnimId;
}

void GameObject::AddChild(const GameObject& child)
{
	m_children.push_back(child);
}

void GameObject::RemoveChild(GameObject& child)
{
	std::vector<GameObject>::iterator it = std::find_if(m_children.begin(), m_children.end(), [&](const GameObject& rhs) { return rhs.GetTypeId() == child.GetTypeId() && rhs.GetId() == child.GetId(); });
	if (it != m_children.end())
	{
		m_children.erase(it);
	}
}

const std::vector<GameObject>& GameObject::GetChildren() const
{
	return m_children;
}

std::vector<GameObject>& GameObject::GetChildren()
{
	return m_children;
}

GameObjectVariable& GameObject::AddVariable()
{
	m_variables.push_back(GameObjectVariable());
	return m_variables[m_variables.size() - 1];
}

void GameObject::RemoveVariable(const GameObjectVariable& variable)
{
	for(int i = 0; i < m_variables.size(); i++)
	{
		if(m_variables[i].m_name == variable.m_name)
		{
			std::swap(m_variables[i], m_variables.back());
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

const GameObjectVariable* GameObject::FindVariable(const std::string& name, int componentIdx) const
{
	for (int i = 0; i < m_variables.size(); i++)
	{
		if (((componentIdx == -1) || (componentIdx == m_variables[i].m_componentIdx)) && ion::string::CompareNoCase(m_variables[i].m_name, name))
		{
			return &m_variables[i];
		}
	}

	return nullptr;
}

GameObjectVariable* GameObject::FindVariable(const std::string& name, int componentIdx)
{
	for (int i = 0; i < m_variables.size(); i++)
	{
		if (((componentIdx == -1) || (componentIdx == m_variables[i].m_componentIdx)) && ion::string::CompareNoCase(m_variables[i].m_name, name))
		{
			return &m_variables[i];
		}
	}

	return nullptr;
}

const GameObjectVariable* GameObject::FindVariableByTag(const std::string& tag, int componentIdx) const
{
	for (int i = 0; i < m_variables.size(); i++)
	{
		if (((componentIdx == -1) || (componentIdx == m_variables[i].m_componentIdx)) && m_variables[i].HasTag(tag))
		{
			return &m_variables[i];
		}
	}

	return nullptr;
}

void GameObject::Serialise(ion::io::Archive& archive)
{
	archive.Serialise(m_objectId, "objectId");
	archive.Serialise(m_typeId, "typeId");
	archive.Serialise(m_spriteActorId, "spriteActorId");
	archive.Serialise(m_spriteSheetId, "spriteSheetId");
	archive.Serialise(m_spriteAnimId, "spriteAnimId");
	archive.Serialise(m_name, "name");
	archive.Serialise(m_position, "position");
	archive.Serialise(m_dimensions, "dimensions");
	archive.Serialise(m_children, "children");
	archive.Serialise(m_variables, "variables");
	archive.Serialise(m_originalArchetypeId, "originalArchetypeId");
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
		bool overrideLine = ParseValueTokens(valueString, templateVariables[i].m_name, (GameObjectVariableSize)templateVariables[i].m_size, gameObjectType);

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
		bool overrideLine = ParseValueTokens(valueString, instanceVariables[i].m_name, (GameObjectVariableSize)instanceVariables[i].m_size, gameObjectType);

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

bool GameObject::ParseValueTokens(std::string& valueString, const std::string& varName, GameObjectVariableSize size, const GameObjectType& gameObjectType) const
{
	const std::string worldPosXString("WORLDPOSX");
	const std::string worldPosYString("WORLDPOSY");
	const std::string widthString("WIDTH");
	const std::string heightString("HEIGHT");
	const std::string ntscString("NTSC(");

	std::string::size_type tokenStart = valueString.find("&");

	if(tokenStart != std::string::npos)
	{
		for (int i = 0; i < s_autoVars.size(); i++)
		{
			std::string::size_type labelStart = valueString.find(s_autoVars[i].label, tokenStart);
			if (labelStart != std::string::npos)
			{
				u32 value = s_autoVars[i].fetchFunc(*this, gameObjectType);

				if (value != 0 || s_autoVars[i].exportIfZero)
				{
					std::stringstream stream;
					stream << "0x" << std::hex << std::setfill('0');

					if (size == eSizeByte)
					{
						stream << std::setw(2);
					}
					else if (size == eSizeWord)
					{
						stream << std::setw(4);
					}
					else if (size == eSizeLong)
					{
						stream << std::setw(8);
					}

					stream << value;

					valueString.replace(labelStart - 1, s_autoVars[i].label.size() + 1, stream.str());
				}
				else
				{
					valueString = "";
					return true;
				}

				return false;
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
