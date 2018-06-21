#include <core/Types.h>
#include <audio/Callback.h>

namespace ion
{
	namespace audio
	{
		class Buffer;
		class Source;

		class Voice : public SourceCallback
		{
			friend class Engine;

		public:
			enum State
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

			virtual u32 GetBufferedBytes() = 0;
			virtual u64 GetPositionSamples() = 0;
			virtual double GetPositionSeconds() = 0;

			State GetState() const;

			//Properties
			virtual void SetVolume(float volume);
			virtual void SetPitch(float pitch);

			float GetVolume() const;
			float GetPitch() const;

		protected:
			Voice(Source& source, bool loop);
			virtual ~Voice();

			virtual void Update() = 0;

			Source& mSource;
			State mState;
			bool mLoop;

			float m_volume;
			float m_pitch;
		};
	}
}