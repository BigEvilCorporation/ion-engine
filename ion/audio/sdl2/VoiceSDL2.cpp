#include <core/debug/Debug.h>
#include <core/memory/Memory.h>
#include <maths/Maths.h>
#include <audio/Buffer.h>
#include <audio/StreamDesc.h>
#include <audio/Source.h>

#include "VoiceSDL2.h"

#define ION_SDL_USE_CALLBACK 1

namespace ion
{
	namespace audio
	{
		VoiceSDL2::VoiceSDL2(Source& source, bool loop)
			: Voice(source, loop)
		{
			m_currentBuffer = NULL;
			m_bufferPos = 0;
			m_startTime = 0;
			m_pauseTime = 0;

			//Get stream desc
			const StreamDesc* streamDesc = source.GetStreamDesc();

			//Create audio spec
#if ION_SDL_USE_CALLBACK
			m_sdlAudioSpec.callback = SDLDataCallback;
#else
			m_sdlAudioSpec.callback = NULL;
#endif
			m_sdlAudioSpec.userdata = this;
			m_sdlAudioSpec.format = CreateFormatFlags(*streamDesc);
			m_sdlAudioSpec.channels = streamDesc->GetNumChannels();
			m_sdlAudioSpec.freq = streamDesc->GetSampleRate();
			m_sdlAudioSpec.samples = streamDesc->GetSizeSamples();

			SDL_AudioSpec createdSpec;

			//Create hardware voice
			m_sdlVoiceId = SDL_OpenAudioDevice(NULL, 0, &m_sdlAudioSpec, &createdSpec, 0);
			if (m_sdlVoiceId <= 0)
			{
				debug::error << "VoiceSDL2::VoiceSDL2() - SDL_OpenAudioDevice() failed with error: " << SDL_GetError() << debug::end;
			}

			//Request first buffers
			for (int i = 0; i < s_numInitialBuffers; i++)
			{
				source.RequestBuffer(*this);
			}

			//Set default properties
			SetVolume(1.0f);
			SetPitch(1.0f);
		}

		VoiceSDL2::~VoiceSDL2()
		{
		}

		void VoiceSDL2::SubmitBuffer(Buffer& buffer)
		{
#if ION_SDL_USE_CALLBACK
			Buffer* bufferPtr = &buffer;
			m_bufferQueue.Push(bufferPtr);
#else
			//Pass data directly to SDL
			buffer.Lock();
			if (SDL_QueueAudio(m_sdlVoiceId, buffer.Get(0), buffer.GetDataSize()) != 0)
			{
				debug::error << "VoiceSDL2::SubmitBuffer() - SDL_QueueAudio() failed: " << SDL_GetError();
			}
			buffer.Unlock();
#endif
		}

		void VoiceSDL2::Play()
		{
			m_startTime = time::GetSystemTicks();
			SDL_PauseAudioDevice(m_sdlVoiceId, 0);
			m_state = Playing;
		}

		void VoiceSDL2::Stop()
		{
			SDL_PauseAudioDevice(m_sdlVoiceId, 1);
			m_state = Stopped;
		}

		void VoiceSDL2::Pause()
		{
			m_pauseTime = time::GetSystemTicks();
			SDL_PauseAudioDevice(m_sdlVoiceId, 1);
			m_state = Paused;
		}

		void VoiceSDL2::Resume()
		{
			m_startTime += (time::GetSystemTicks() - m_pauseTime);
			SDL_PauseAudioDevice(m_sdlVoiceId, 0);
			m_state = Playing;
		}

		u32 VoiceSDL2::GetBufferedBytes()
		{
			return SDL_GetQueuedAudioSize(m_sdlVoiceId);
		}

		u32 VoiceSDL2::GetConsumedBytes()
		{
			const StreamDesc* streamDesc = m_source.GetStreamDesc();
			return GetPositionSamples() * (streamDesc->GetBitsPerSample() / 8) * streamDesc->GetNumChannels();
		}

		u64 VoiceSDL2::GetPositionSamples()
		{
			return (u64)(GetPositionSeconds() * (double)m_source.GetStreamDesc()->GetSampleRate());
		}

		double VoiceSDL2::GetPositionSeconds()
		{
			if (m_state == Playing)
			{
				return time::TicksToSeconds(time::GetSystemTicks() - m_startTime);
			}
			else
			{
				return time::TicksToSeconds(m_pauseTime - m_startTime);
			}
		}

		void VoiceSDL2::Update()
		{
		}

		void VoiceSDL2::SetVolume(float volume)
		{
			//mSDL2Voice->SetVolume(volume);
			Voice::SetVolume(volume);
		}

		void VoiceSDL2::SetPitch(float pitch)
		{
			//mSDL2Voice->SetFrequencyRatio(pitch);
			Voice::SetPitch(pitch);
		}

		SDL_AudioFormat VoiceSDL2::CreateFormatFlags(const StreamDesc& streamDesc)
		{
			SDL_AudioFormat format = 0;

			switch (streamDesc.GetBitsPerSample())
			{
			case 8:
				format |= AUDIO_S8;
				break;
			case 16:
				format |= AUDIO_S16;
				break;
			case 32:
				format |= AUDIO_S32;
				break;
			default:
				debug::error << "VoiceSDL2::CreateFormatFlags() - Unsupported bits per sample: " << streamDesc.GetBitsPerSample() << debug::end;
			}

			return format;
		}

		void VoiceSDL2::SDLDataCallback(void* userdata, Uint8* stream, int len)
		{
			VoiceSDL2* sdl2Voice = (VoiceSDL2*)userdata;
			sdl2Voice->SDLFillBuffer(stream, len);
		}

		void VoiceSDL2::SDLFillBuffer(Uint8* stream, int len)
		{
			ion::debug::log << "SDL wants " << len << " bytes, " << GetConsumedBytes() << " bytes played" << ion::debug::end;

			if (!m_currentBuffer && !m_bufferQueue.IsEmpty())
			{
				//Pop next buffer
				m_currentBuffer = m_bufferQueue.Pop();
				m_currentBuffer->Lock();
			}

			if (m_currentBuffer)
			{
				int bytesRemaining = len;

				while (bytesRemaining > 0)
				{
					int bytesToCopy = maths::Min(bytesRemaining, (int)m_currentBuffer->GetDataSize() - m_bufferPos);
					memory::MemCopy(stream, m_currentBuffer->Get(m_bufferPos), bytesToCopy);

					bytesRemaining -= bytesToCopy;
					m_bufferPos += bytesToCopy;

					if (m_bufferPos == m_currentBuffer->GetDataSize())
					{
						//Unlock buffer
						m_currentBuffer->Unlock();

						//Reset read head
						m_bufferPos = 0;
						m_currentBuffer = NULL;

						//Request new buffer
						m_source.RequestBuffer(*this);

						if (m_bufferQueue.IsEmpty())
						{
							//Starved
							bytesRemaining = 0;
						}
						else
						{
							//Pop next buffer
							m_currentBuffer = m_bufferQueue.Pop();
							m_currentBuffer->Lock();
						}
					}
				}
			}
			else
			{
				debug::log << "VoiceSDL2::SDLFillBuffer() - Buffer starved" << debug::end;
			}
		}
	}
}
