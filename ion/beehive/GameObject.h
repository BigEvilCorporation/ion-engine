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

#include <ion/maths/Vector.h>
#include <ion/core/io/Serialise.h>
#include <ion/core/string/String.h>

#include <vector>
#include <sstream>
#include <string>
#include <functional>

#include "SpriteSheet.h"
#include "Types.h"

class GameObject;

enum GameObjectVariableSize
{
	eSizeByte = 1,
	eSizeWord = 2,
	eSizeLong = 4
};

struct GameObjectVariable
{
	GameObjectVariable()
	{
		m_name = "GameObj_";
		m_value = "0";
		m_componentIdx = -1;
		m_size = 1;
	}

	void Serialise(ion::io::Archive& archive)
	{
		archive.Serialise(m_name, "name");
		archive.Serialise(m_value, "value");
		archive.Serialise(m_componentName, "componentName");
		archive.Serialise(m_componentIdx, "componentIdx");
		archive.Serialise(m_size, "size");
		archive.Serialise(m_tags, "mtags");
	}

	bool HasTag(const std::string& name) const
	{
		for (int i = 0; i < m_tags.size(); i++)
		{
			if (ion::string::CompareNoCase(m_tags[i], name))
			{
				return true;
			}
		}

		return false;
	}

	bool FindTagValue(const std::string& name, std::string& value) const
	{
		std::string searchToken = name + "_";

		for (int i = 0; i < m_tags.size(); i++)
		{
			if (ion::string::StartsWith(m_tags[i], searchToken))
			{
				value = ion::string::RemoveSubstring(m_tags[i], searchToken);
				return true;
			}
		}

		return false;
	}

	std::string m_name;
	std::string m_value;
	std::string m_componentName;
	s8 m_componentIdx;
	u8 m_size;
	std::vector<std::string> m_tags;
};

struct GameObjectScriptFunc
{
	std::string routine;
	std::string name;
	std::string returnType;
	std::vector<std::pair<std::string, std::string>> params;
	s8 componentIdx;

	bool operator == (const GameObjectScriptFunc& rhs) const
	{
		return routine == rhs.routine
			&& name == rhs.routine
			&& returnType == rhs.returnType
			&& params == rhs.params
			&& componentIdx == rhs.componentIdx;
	}

	void Serialise(ion::io::Archive& archive)
	{
		archive.Serialise(routine, "routine");
		archive.Serialise(name, "name");
		archive.Serialise(returnType, "returnType");
		archive.Serialise(params, "params");
		archive.Serialise(componentIdx, "componentIdx");
	}
};

struct GameObjectArchetype
{
	GameObjectArchetype()
	{
		id = InvalidGameObjectArchetypeId;
		typeId = InvalidGameObjectTypeId;
		spriteActorId = InvalidActorId;
		spriteSheetId = InvalidSpriteSheetId;
		spriteAnimId = InvalidSpriteAnimId;
	}

	void Serialise(ion::io::Archive& archive)
	{
		archive.Serialise(id, "id");
		archive.Serialise(typeId, "typeId");
		archive.Serialise(name, "name");
		archive.Serialise(variables, "variables");
		archive.Serialise(spriteActorId, "spriteActorId");
		archive.Serialise(spriteSheetId, "spriteSheetId");
		archive.Serialise(spriteAnimId, "spriteAnimId");
	}

	const GameObjectVariable* FindVariable(const std::string& name, int componentIdx) const
	{
		for (int i = 0; i < variables.size(); i++)
		{
			if (((componentIdx == -1) || (componentIdx == variables[i].m_componentIdx)) && ion::string::CompareNoCase(variables[i].m_name, name))
			{
				return &variables[i];
			}
		}

		return nullptr;
	}

	const GameObjectVariable* FindVariableByTag(const std::string& tag, int componentIdx) const
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

	GameObjectArchetypeId id;
	GameObjectTypeId typeId;
	std::string name;
	std::vector<GameObjectVariable> variables;
	ActorId spriteActorId;
	SpriteSheetId spriteSheetId;
	SpriteAnimId spriteAnimId;
};

typedef std::map<GameObjectArchetypeId, GameObjectArchetype> TGameObjectArchetypeMap;

class GameObjectType
{
public:
	struct PrefabChild
	{
		std::string name;
		GameObjectTypeId typeId;
		GameObjectId instanceId;
		ion::Vector2i relativePos;
		ion::Vector2i dimensions;

		ActorId spriteActorId;
		SpriteSheetId spriteSheetId;
		SpriteAnimId spriteAnimId;
		
		std::vector<GameObjectVariable> variables;

		const GameObjectVariable* FindVariableByTag(const std::string& tag, int componentIdx = -1) const;

		//Interface for animation timeline
		const ion::Vector2i& GetPosition() const { return relativePos; }
		void SetPosition(const ion::Vector2i& position) { relativePos = position; }
		ActorId GetSpriteActorId() const { return spriteActorId; }
		void SetSpriteActorId(ActorId id) { spriteActorId = id; }
		SpriteSheetId GetSpriteSheetId() const { return spriteSheetId; }
		void SetSpriteSheetId(SpriteSheetId id) { spriteSheetId = id; }
		SpriteAnimId GetSpriteAnim() const { return spriteAnimId; }
		void SetSpriteAnim(SpriteAnimId id) { spriteAnimId = id; }

		void Serialise(ion::io::Archive& archive)
		{
			archive.Serialise(name, "name");
			archive.Serialise(typeId, "typeId");
			archive.Serialise(instanceId, "instanceId");
			archive.Serialise(relativePos, "relativePos");
			archive.Serialise(dimensions, "dimensions");
			archive.Serialise(spriteActorId, "spriteActorId");
			archive.Serialise(spriteSheetId, "spriteSheetId");
			archive.Serialise(spriteAnimId, "spriteAnimId");
			archive.Serialise(variables, "variables");
		}
	};

	GameObjectType();
	GameObjectType(GameObjectTypeId id);

	GameObjectTypeId GetId() const { return m_id; }

	//Type name
	void SetName(const std::string& name) { m_name = name; }
	const std::string& GetName() const { return m_name; }

	//Prefab name
	void SetPrefabName(const std::string& name) { m_prefabName = name; }
	const std::string& GetPrefabName() const { return m_prefabName; }

	//Prefab hierarchy
	void AddPrefabChild(const PrefabChild& child);
	void RemovePrefabChild(PrefabChild& Child);
	PrefabChild* FindPrefabChild(GameObjectId instanceId);
	const std::vector<PrefabChild>& GetPrefabChildren() const;
	std::vector<PrefabChild>& GetPrefabChildren();

	bool LoadPreviewSprite(const std::string& filename);
	SpriteSheetId GetPreviewSpriteSheetId() const { return m_previewSpriteSheetId; }
	const SpriteSheet& GetPreviewSpriteSheet() const { return m_previewSpriteSheet; }

	//Sprite
	ActorId GetSpriteActorId() const { return m_spriteActorId; }
	void SetSpriteActorId(ActorId id) { m_spriteActorId = id; }
	SpriteSheetId GetSpriteSheetId() const { return m_spriteSheetId; }
	void SetSpriteSheetId(SpriteSheetId id) { m_spriteSheetId = id; }

	//Animation
	//TODO: Poll from current anim+track during rendering, don't store here
	SpriteAnimId GetSpriteAnim() const { return m_spriteAnimId; }
	void SetSpriteAnim(SpriteAnimId animId) { m_spriteAnimId = animId; }

	const ion::Vector2i& GetDimensions() const { return m_dimensions; }
	void SetDimensions(const ion::Vector2i& dimensions) { m_dimensions = dimensions; }

	//Archetypes
	const GameObjectArchetype* GetArchetype(GameObjectArchetypeId id) const;
	GameObjectArchetype* GetArchetype(GameObjectArchetypeId id);
	const GameObjectArchetype* GetArchetype(const std::string& name) const;
	GameObjectArchetypeId AddArchetype(GameObjectArchetype& archetype);
	GameObjectArchetypeId CreateArchetypeFromState(const GameObject& gameObject);
	const TGameObjectArchetypeMap& GetArchetypes() const;

	//Variables
	GameObjectVariable& AddVariable();
	void RemoveVariable(GameObjectVariable& variable);
	void ClearVariables();
	GameObjectVariable* GetVariable(u32 index);
	const GameObjectVariable* GetVariable(u32 index) const;
	GameObjectVariable* FindVariable(const std::string& name, int componentIdx = -1);
	const GameObjectVariable* FindVariable(const std::string& name, int componentIdx = -1) const;
	const GameObjectVariable* FindVariableByTag(const std::string& tag, int componentIdx = -1) const;
	const std::vector<GameObjectVariable>& GetVariables() const { return m_variables; }
	std::vector<GameObjectVariable>& GetVariables() { return m_variables; }

	//Script variables (definitions only, no values)
	void AddScriptVariable(const GameObjectVariable& variable) { m_scriptVariables.push_back(variable); }
	void ClearScriptVariables() { m_scriptVariables.clear(); }
	const std::vector<GameObjectVariable>& GetScriptVariables() const { return m_scriptVariables; }

	//Script functions
	void AddScriptFunction(const GameObjectScriptFunc& scriptFunc) { m_scriptFunctions.push_back(scriptFunc); }
	void ClearScriptFunctions() { m_scriptFunctions.clear(); }
	const std::vector<GameObjectScriptFunc>& GetScriptFunctions() const { return m_scriptFunctions; }

	int GetInitPriority() const { return m_initPriority; }
	void SetInitPriority(int priority) { m_initPriority = priority; }

	bool IsStaticType() const { return m_static; }
	void SetStatic(bool isStatic) { m_static = isStatic; }

	bool IsPrefabType() const { return m_prefab; }
	void SetPrefab(bool isPrefab) { m_prefab = isPrefab; }

	void Serialise(ion::io::Archive& archive);

private:
	GameObjectTypeId m_id;
	std::string m_name;
	std::string m_prefabName;
	bool m_static;
	bool m_prefab;
	ActorId m_spriteActorId;
	SpriteSheetId m_spriteSheetId;
	SpriteAnimId m_spriteAnimId;
	SpriteSheetId m_previewSpriteSheetId;
	SpriteSheet m_previewSpriteSheet;
	std::vector<GameObjectVariable> m_variables;
	std::vector<GameObjectVariable> m_scriptVariables;
	std::vector<GameObjectScriptFunc> m_scriptFunctions;
	std::vector<PrefabChild> m_prefabChildren;
	ion::Vector2i m_dimensions;
	int m_initPriority;
	TGameObjectArchetypeMap m_archetypes;
};

class GameObject
{
public:
	static const int screenToWorldSpaceShift = 16;
	static const int spriteSheetBorderX = 128;
	static const int spriteSheetBorderY = 128;

	struct AutoVar
	{
		typedef std::function<u32(const GameObject& gameObject, const GameObjectType& gameObjectType)> FetchValue;

		std::string name;
		std::string label;
		std::string prefix;
		GameObjectVariableSize size;
		FetchValue fetchFunc;
		bool exportIfZero;
	};

	static const std::vector<AutoVar> s_autoVars;

	GameObject();
	GameObject(GameObjectId objectId, const GameObject& rhs);
	GameObject(GameObjectId objectId, GameObjectTypeId typeId, const GameObject& rhs);
	GameObject(GameObjectId objectId, GameObjectTypeId typeId, const ion::Vector2i& position);
	GameObject(GameObjectId objectId, GameObjectTypeId typeId, const ion::Vector2i& position, const ion::Vector2i& dimensions);

	GameObjectId GetId() const { return m_objectId; }
	GameObjectTypeId GetTypeId() const { return m_typeId; }

	void ApplyArchetype(const GameObjectArchetype& archetype);
	GameObjectArchetypeId GetOriginalArchetype() const { return m_originalArchetypeId; }

	void SetName(const std::string& name) { m_name = name; }
	const std::string& GetName() const { return m_name; }

	const ion::Vector2i& GetDimensions() const { return m_dimensions; }
	void SetDimensions(const ion::Vector2i& dimensions) { m_dimensions = dimensions; }

	const ion::Vector2i& GetPosition() const { return m_position; }
	void SetPosition(const ion::Vector2i& position) { m_position = position; }

	//Hierarchy
	void AddChild(const GameObject& child);
	void RemoveChild(GameObject& Child);
	const std::vector<GameObject>& GetChildren() const;
	std::vector<GameObject>& GetChildren();

	//Sprite
	ActorId GetSpriteActorId() const { return m_spriteActorId; }
	void SetSpriteActorId(ActorId id) { m_spriteActorId = id; }
	SpriteSheetId GetSpriteSheetId() const { return m_spriteSheetId; }
	void SetSpriteSheetId(SpriteSheetId id) { m_spriteSheetId = id; }

	//Animation
	//TODO: Poll from current anim+track during rendering, don't store here
	SpriteAnimId GetSpriteAnim() const { return m_spriteAnimId; }
	void SetSpriteAnim(SpriteAnimId animId) { m_spriteAnimId = animId; }

	GameObjectVariable& AddVariable();
	void RemoveVariable(const GameObjectVariable& variable);
	GameObjectVariable* GetVariable(u32 index);
	const GameObjectVariable* FindVariable(const std::string& name, int componentIdx = -1) const;
	GameObjectVariable* FindVariable(const std::string& name, int componentIdx = -1);
	const GameObjectVariable* FindVariableByTag(const std::string& tag, int componentIdx = -1) const;
	const std::vector<GameObjectVariable>& GetVariables() const { return m_variables; }
	std::vector<GameObjectVariable>& GetVariables() { return m_variables; }

	void Serialise(ion::io::Archive& archive);
	void Export(std::stringstream& stream, const GameObjectType& gameObjectType, const std::string& name, int mapWidth) const;

private:
	bool ParseValueTokens(std::string& valueString, const std::string& varName, GameObjectVariableSize size, const GameObjectType& gameObjectType) const;

	GameObjectId m_objectId;
	GameObjectTypeId m_typeId;
	std::string m_name;
	ion::Vector2i m_position;
	ion::Vector2i m_dimensions;
	std::vector<GameObject> m_children;
	std::vector<GameObjectVariable> m_variables;
	GameObjectArchetypeId m_originalArchetypeId;

	//Sprite
	SpriteSheetId m_spriteSheetId;
	ActorId m_spriteActorId;

	//Animation
	//TODO: Poll from current anim+track during rendering, don't store here
	SpriteAnimId m_spriteAnimId;
};