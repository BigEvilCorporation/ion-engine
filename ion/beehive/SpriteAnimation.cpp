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

#include "SpriteAnimation.h"

#include <iostream>
#include <iomanip>

#define HEX2TRUNC(val) std::hex << std::setfill('0') << std::setw(2) << std::uppercase << ((int)val&0xFF)

SpriteAnimation::SpriteAnimation()
{

}

void SpriteAnimation::SetName(const std::string& name)
{
	m_name = name;
}

const std::string& SpriteAnimation::GetName() const
{
	return m_name;
}

void SpriteAnimation::Serialise(ion::io::Archive& archive)
{
	ion::render::Animation::Serialise(archive);

	archive.Serialise(m_name, "name");
	archive.Serialise(m_trackSpriteFrame, "trackSpriteFrame");
	archive.Serialise(m_trackSFX, "trackSFX");
	archive.Serialise(m_trackPosition, "trackPosition");

	if(archive.GetDirection() == ion::io::Archive::Direction::In)
	{
		//Legacy
		int speed = 1;
		archive.Serialise(speed, "speed");

		if(speed != 1)
		{
			SetPlaybackSpeed((float)speed / 4.6f);
		}
	}
}

const u32 AnimTrackSpriteFrame::GetValue(float time) const
{
	u32 intValue = 0;

	if(const ion::render::Keyframe<u32>* keyframeA = GetPrevKeyframe(time))
	{
		intValue = keyframeA->GetValue();
	}

	return intValue;
}

void AnimTrackSpriteFrame::Export(std::stringstream& stream, const std::string& actorName, const std::string& sheetName) const
{
	for(int i = 0; i < GetNumKeyframes(); i++)
	{
		u32 value = GetKeyframe(i).GetValue();
		std::stringstream label;
		label << "actor_" << actorName << "_sheet_" << sheetName << "_frame_" << value << "_header";
		stream << "\tdc.l " << label.str() << std::endl;
	}

	stream << std::endl;
}

void AnimTrackSpriteFrame::Export(ion::io::File& file) const
{

}

AnimTrackSpritePosition::AnimTrackSpritePosition()
{
	//Linear mode by default
	SetBlendMode(BlendMode::Linear);
}

const ion::Vector2i AnimTrackSpritePosition::GetValue(float time) const
{
	ion::Vector2i result;

	const AnimKeyframePosition* keyframeA = GetPrevKeyframe(time);
	const AnimKeyframePosition* keyframeB = GetNextKeyframe(time);

	if(keyframeA && keyframeB)
	{
		if(GetBlendMode() == BlendMode::Snap)
		{
			result = keyframeA->GetValue();
		}
		else
		{
			float timeA = keyframeA->GetTime();
			float timeB = keyframeB->GetTime();

			float lerpTime = ion::maths::UnLerp(timeA, timeB, time);

			const ion::Vector2i& vectorA = keyframeA->GetValue();
			const ion::Vector2i& vectorB = keyframeB->GetValue();

			result = vectorA.Lerp(vectorB, lerpTime);
		}
	}

	return result;
}

void AnimTrackSpritePosition::ExportX(std::stringstream& stream, int numKeyframes) const
{
	stream << "\tdc.b ";

	for(int i = 0; i < numKeyframes; i++)
	{
		u32 value = (i < GetNumKeyframes()) ? GetKeyframe(i).GetValue().x : 0;
		stream << "0x" << HEX2TRUNC(value);

		if(i < numKeyframes - 1)
			stream << ", ";
	}

	stream << std::endl;
}

void AnimTrackSpritePosition::ExportY(std::stringstream& stream, int numKeyframes) const
{
	stream << "\tdc.b ";

	for(int i = 0; i < numKeyframes; i++)
	{
		u32 value = (i < GetNumKeyframes()) ? GetKeyframe(i).GetValue().y : 0;
		stream << "0x" << HEX2TRUNC(value);

		if(i < numKeyframes - 1)
			stream << ", ";
	}

	stream << std::endl;
}

void AnimTrackSpritePosition::ExportX(ion::io::File& file, int numKeyframes) const
{

}

void AnimTrackSpritePosition::ExportY(ion::io::File& file, int numKeyframes) const
{

}

const std::string AnimTrackSFX::GetValue(float time) const
{
	std::string value;

	if(const ion::render::Keyframe<std::string>* keyframeA = GetPrevKeyframe(time))
	{
		value = keyframeA->GetValue();
	}

	return value;
}

void AnimTrackSFX::Export(std::stringstream& stream, int numKeyframes) const
{
	stream << "\tdc.l ";

	for(int i = 0; i < numKeyframes; i++)
	{
		std::string value = (i < GetNumKeyframes()) ? GetKeyframe(i).GetValue() : "";

		if(value.size() > 0)
		{
			stream << value;
		}
		else
		{
			stream << "0";
		}

		if(i < numKeyframes - 1)
			stream << ", ";
	}

	stream << std::endl;
}

void AnimTrackSFX::Export(ion::io::File& file, int numKeyframes) const
{

}
