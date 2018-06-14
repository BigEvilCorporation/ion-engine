#include "Voice.h"

namespace ion
{
	namespace audio
	{
		Voice::Voice(Source& source, bool loop)
			: mSource(source)
			, mLoop(loop)
		{
			m_volume = 0.0f;
			m_pitch = 0.0f;
		}

		Voice::~Voice()
		{
		}

		Voice::State Voice::GetState() const
		{
			return mState;
		}

		void Voice::SetVolume(float volume)
		{
			m_volume = volume;
		}

		void Voice::SetPitch(float pitch)
		{
			m_pitch = pitch;
		}

		float Voice::GetVolume() const
		{
			return m_volume;
		}

		float Voice::GetPitch() const
		{
			return m_pitch;
		}
	}
}