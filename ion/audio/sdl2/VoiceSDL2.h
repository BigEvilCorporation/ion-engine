#include <audio/Voice.h>
#include <audio/Buffer.h>
#include <audio/StreamDesc.h>
#include <core/containers/Queue.h>
#include <core/time/Time.h>

#include <SDL2/SDL.h>

namespace ion
{
	namespace audio
	{
		class VoiceSDL2 : public Voice
		{
			friend class EngineSDL2;
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
			VoiceSDL2(Source& source, bool loop);
			virtual ~VoiceSDL2();

			virtual void Update();

		private:
			static const int s_starvedBufferSize = 1024;

			static SDL_AudioFormat CreateFormatFlags(const StreamDesc& streamDesc);
			static void SDLDataCallback(void* userdata, Uint8* stream, int len);
			void SDLFillBuffer(Uint8* stream, int len);

			SDL_AudioSpec m_sdlAudioSpec;
			SDL_AudioDeviceID m_sdlVoiceId;

			u64 m_startTime;
			u64 m_pauseTime;

			static const int s_bufferQueueSize = 2;
			Queue<Buffer*, s_bufferQueueSize> m_bufferQueue;
			Buffer* m_currentBuffer;
			int m_bufferPos;
		};
	}
}