#pragma once

namespace ion
{
	namespace audio
	{
		class Voice;

		class Effect
		{
		protected:
			Effect() {}
			virtual ~Effect() {}

			virtual void Update(float deltaTime) = 0;
			virtual void Apply(Voice* voice) = 0;

			friend class Engine;
			friend class Voice;
		};
	}
}