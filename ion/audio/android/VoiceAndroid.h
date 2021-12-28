#include <audio/Voice.h>

namespace ion
{
	namespace audio
	{
		class VoiceAndroid : public Voice
		{
			friend class EngineAndroid;
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
			VoiceAndroid(Source& source, bool loop);
			virtual ~VoiceAndroid();

			virtual void Update();
		};
	}
}