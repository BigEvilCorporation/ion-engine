#include <core/Types.h>
#include <core/thread/CriticalSection.h>
#include <audio/Callback.h>

#include <vector>

namespace ion
{
	namespace audio
	{
		class Buffer;
		class Source;
		class Effect;

		class Voice : public SourceCallback
		{
			friend class Engine;

		public:
			enum class State
			{
				Playing,
				Paused,
				Stopped
			};

			//Transport
			virtual void Play() = 0;
			virtual void Stop() = 0;
			virtual void Pause() = 0;
			virtual void Resume() = 0;

			virtual u32 GetQueuedBuffers() = 0;
			virtual u32 GetBufferedBytes() = 0;
			virtual u32 GetConsumedBytes() = 0;
			virtual u64 GetPositionSamples() = 0;
			virtual double GetPositionSeconds() = 0;

			State GetState() const;

			//Properties
			virtual void SetVolume(float volume);
			virtual void SetPitch(float pitch);

			float GetVolume() const;
			float GetPitch() const;

			//Effects
			template <typename T> T* CreateEffect();
			void DestroyEffect(Effect& effect);

		protected:
			Voice(Source& source, bool loop);
			virtual ~Voice();

			virtual void Update(float deltaTime);

			Source& m_source;
			State m_state;
			bool m_loop;

			float m_volume;
			float m_pitch;

			ion::thread::CriticalSection m_effectsListCritSec;

			std::vector<Effect*> m_effects;
		};

		template <typename T> T* Voice::CreateEffect()
		{
			T* effect = new T();
			m_effectsListCritSec.Begin();
			m_effects.push_back(effect);
			m_effectsListCritSec.End();
			return effect;
		}
	}
}