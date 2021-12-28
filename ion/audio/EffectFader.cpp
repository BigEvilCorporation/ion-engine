#include "EffectFader.h"
#include "Voice.h"

#include <ion/maths/Maths.h>

namespace ion
{
	namespace audio
	{
		EffectFader::EffectFader()
		{
			m_volume = 0.0f;
			m_speed = 0.0f;
		}

		EffectFader::~EffectFader()
		{

		}

		void EffectFader::FadeIn(float speed, OnFadeFinished const& onFinished)
		{
			m_speed = speed;
			m_onFinished = onFinished;

			if (m_volume == 1.0f && onFinished)
			{
				onFinished(*this);
			}
		}

		void EffectFader::FadeOut(float speed, OnFadeFinished const& onFinished)
		{
			m_speed = -speed;
			m_onFinished = onFinished;

			if (m_volume == 0.0f && onFinished)
			{
				onFinished(*this);
			}
		}

		void EffectFader::Update(float deltaTime)
		{
			float prev = m_volume;
			m_volume = ion::maths::Clamp(m_volume + (m_speed * deltaTime), 0.0f, 1.0f);

			if (m_volume != prev && (m_volume == 0.0f || m_volume == 1.0f))
			{
				if (m_onFinished)
				{
					m_onFinished(*this);
				}
			}
		}

		void EffectFader::Apply(Voice* voice)
		{
			voice->SetVolume(m_volume);
		}
	}
}