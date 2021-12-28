#include "Voice.h"
#include "Source.h"
#include "Effect.h"

#include <ion/core/utils/STL.h>

namespace ion
{
	namespace audio
	{
		Voice::Voice(Source& source, bool loop)
			: m_source(source)
		{
			m_state = State::Stopped;
			m_volume = 0.0f;
			m_pitch = 0.0f;

			//If a streaming feed, it's up to the feed to handle looping, not the voice
			if (source.GetFeedType() == Source::FeedType::Streaming)
			{
				m_loop = false;
			}
			else
			{
				m_loop = loop;
			}
		}

		Voice::~Voice()
		{
			for (int i = 0; i < m_effects.size(); i++)
			{
				delete m_effects[i];
			}
		}

		void Voice::Update(float deltaTime)
		{
			if (m_state == State::Playing)
			{
				m_effectsListCritSec.Begin();

				for (int i = 0; i < m_effects.size(); i++)
				{
					m_effects[i]->Update(deltaTime);
					m_effects[i]->Apply(this);
				}

				m_effectsListCritSec.End();
			}
		}

		Voice::State Voice::GetState() const
		{
			return m_state;
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

		void Voice::DestroyEffect(Effect& effect)
		{
			m_effectsListCritSec.Begin();
			ion::utils::stl::FindAndRemove(m_effects, &effect);
			m_effectsListCritSec.End();
			delete &effect;
		}
	}
}