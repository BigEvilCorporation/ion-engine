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

#include <ion/core/Types.h>
#include <ion/maths/Vector.h>
#include <ion/core/cryptography/UUID.h>
#include <ion/renderer/Animation.h>

#include <sstream>

#include "Types.h"
#include "Animation.h"

//Keyframes
typedef ion::render::Keyframe<u32> AnimKeyframeSpriteFrame;
typedef ion::render::Keyframe<ion::Vector2i> AnimKeyframeSpritePosition;
typedef ion::render::Keyframe<std::string> AnimKeyframeSpriteSFX;

//Tracks
class AnimTrackSpriteFrame : public ion::render::AnimationTrack<u32>
{
public:
	const u32 GetValue(float time) const;
	void Export(std::stringstream& stream, const std::string& actorName, const std::string& sheetName) const;
	void Export(ion::io::File& file) const;
};

class AnimTrackSpritePosition : public ion::render::AnimationTrack<ion::Vector2i>
{
public:
	AnimTrackSpritePosition();
	virtual const ion::Vector2i GetValue(float time) const;
	void ExportX(std::stringstream& stream, int numKeyframes) const;
	void ExportY(std::stringstream& stream, int numKeyframes) const;
	void ExportX(ion::io::File& file, int numKeyframes) const;
	void ExportY(ion::io::File& file, int numKeyframes) const;
};

class AnimTrackSFX : public ion::render::AnimationTrack<std::string>
{
public:
	const std::string GetValue(float time) const;
	void Export(std::stringstream& stream, int numKeyframes) const;
	void Export(ion::io::File& file, int numKeyframes) const;
};

class SpriteAnimation : public ion::render::Animation
{
public:
	SpriteAnimation();

	void SetName(const std::string& name);
	const std::string& GetName() const;

	void Serialise(ion::io::Archive& archive);

	AnimTrackSpriteFrame m_trackSpriteFrame;
	AnimTrackSpritePosition m_trackPosition;
	AnimTrackSFX m_trackSFX;

private:
	std::string m_name;
};

typedef std::map<SpriteAnimId, SpriteAnimation> TSpriteAnimMap;