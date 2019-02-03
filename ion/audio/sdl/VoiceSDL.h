#include <audio/Voice.h>
#include <audio/Buffer.h>
#include <audio/StreamDesc.h>
#include <core/containers/Queue.h>
#include <core/time/Time.h>

#include <SDL/SDL.h>

namespace ion
{
	namespace audio
	{
		class VoiceSDL : public Voice
		{
			friend class EngineSDL;
		public:
			//Transport
			virtual void Play();
			virtual void Stop();
			virtual void Pause();
			virtual void Resume();

			virtual u32 GetBufferedBytes();
			virtual u64 GetPositionSamples();
			virtual double GetPositionSeconds();

			//Properties
			virtual void SetVolume(float volume);
			virtual void SetPitch(float pitch);

			//Submit buffer
			virtual void SubmitBuffer(Buffer& buffer);

		protected:
			VoiceSDL(Source& source, bool loop);
			virtual ~VoiceSDL();

			virtual void Update();

		private:
			u64 m_startTime;
		};
	}
}