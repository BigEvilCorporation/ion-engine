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

#include "Types.h"

#include <ion/core/Types.h>
#include <ion/maths/Vector.h>
#include <ion/renderer/Animation.h>
#include <ion/beehive/SpriteAnimation.h>

class SpriteSheet;

//Keyframes
typedef ion::render::Keyframe<ion::Vector2i> AnimKeyframePosition;
typedef ion::render::Keyframe<std::pair<SpriteSheetId, SpriteAnimId>> AnimKeyframeSpriteAnim;

enum AnimationTracks
{
	eTrackPosition,
	eTrackSpriteAnim,

	eTrackCount
};

class AnimTrackPosition : public ion::render::AnimationTrack<ion::Vector2i>
{
public:
	virtual const ion::Vector2i GetValue(float time) const;
	void Export(std::stringstream& stream) const;
	void Export(ion::io::File& file) const;
};

class AnimTrackSpriteAnim : public ion::render::AnimationTrack<std::pair<SpriteSheetId, SpriteAnimId>>
{
public:
	virtual const std::pair<SpriteSheetId, SpriteAnimId> GetValue(float time) const;
	void Export(std::stringstream& stream) const;
	void Export(ion::io::File& file) const;
};

class AnimationActor
{
public:
	AnimationActor();
	AnimationActor(GameObjectId gameObjectId);

	GameObjectId GetGameObjectId() const;

	void Serialise(ion::io::Archive& archive);

	AnimTrackPosition m_trackPosition;
	AnimTrackSpriteAnim m_trackSpriteAnim;

private:
	GameObjectId m_gameObjectId;
};

typedef std::map<GameObjectId, AnimationActor> TAnimActorMap;

class Animation : public ion::render::Animation
{
public:
	Animation();
	Animation(AnimationId animId);

	AnimationId GetId() const { return m_id; }

	void SetName(const std::string& name);
	const std::string& GetName() const;

	const ion::Vector2i& GetAnchor() const { return m_anchor; }
	void SetAnchor(const ion::Vector2i& anchor) { m_anchor = anchor; }

	GameObjectTypeId GetPrefabId() const { return m_prefabId; }
	void SetPrefabId(GameObjectTypeId prefabId) { m_prefabId = prefabId; }

	void AddActor(GameObjectId actorId);
	void RemoveActor(GameObjectId actorId);
	AnimationActor* GetActor(GameObjectId actorId);
	const AnimationActor* GetActor(GameObjectId actorId) const;
	TAnimActorMap::iterator ActorsBegin();
	TAnimActorMap::iterator ActorsEnd();
	TAnimActorMap::const_iterator ActorsBegin() const;
	TAnimActorMap::const_iterator ActorsEnd() const;
	int GetActorCount() const;

	void Serialise(ion::io::Archive& archive);

private:
	AnimationId m_id;
	std::string m_name;
	ion::Vector2i m_anchor;
	GameObjectTypeId m_prefabId;
	TAnimActorMap m_actors;
};
