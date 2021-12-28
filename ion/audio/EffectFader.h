#pragma once

#include "Effect.h"

#include <functional>

namespace ion
{
	namespace audio
	{
		class EffectFader : public Effect
		{
		public:
			typedef std::function<void(EffectFader&)> OnFadeFinished;

			EffectFader();
			virtual ~EffectFader();

			void FadeIn(float speed, OnFadeFinished const& onFinished = nullptr);
			void FadeOut(float speed, OnFadeFinished const& onFinished = nullptr);

		protected:
			virtual void Update(float deltaTime);
			virtual void Apply(Voice* voice);

		private:
			float m_volume;
			float m_speed;
			OnFadeFinished m_onFinished;
		};
	}
}