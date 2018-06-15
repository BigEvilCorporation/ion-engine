#include <core/debug/Debug.h>
#include <core/memory/Memory.h>
#include <maths/Maths.h>
#include <audio/Buffer.h>
#include <audio/StreamDesc.h>
#include <audio/Source.h>

#include "VoiceSDL2.h"

namespace ion
{
	namespace audio
	{
		VoiceSDL2::VoiceSDL2(Source& source, bool loop)
			: Voice(source, loop)
		{
			m_currentBuffer = NULL;
			m_bufferPos = 0;

			//Get stream desc
			const StreamDesc* streamDesc = source.GetStreamDesc();

			//Create audio spec
			m_sdlAudioSpec.callback = SDLDataCallback;
			m_sdlAudioSpec.userdata = this;
			m_sdlAudioSpec.format = CreateFormatFlags(*streamDesc);
			m_sdlAudioSpec.channels = streamDesc->GetNumChannels();
			m_sdlAudioSpec.freq = streamDesc->GetSampleRate();
			m_sdlAudioSpec.samples = streamDesc->GetSizeSamples();
			m_sdlAudioSpec.size = streamDesc->GetDecodedSizeBytes();

			//Create hardware voice
			m_sdlVoiceId = SDL_OpenAudioDevice(NULL, 0, &m_sdlAudioSpec, NULL, 0);
			if (m_sdlVoiceId < 0)
			{
				debug::error << "VoiceSDL2::VoiceSDL2() - SDL_OpenAudioDevice() failed with error: " << SDL_GetError() << debug::end;
			}

			//Request first buffer
			source.RequestBuffer(*this);

			//Set default properties
			SetVolume(1.0f);
			SetPitch(1.0f);
		}

		VoiceSDL2::~VoiceSDL2()
		{
		}

		void VoiceSDL2::SubmitBuffer(Buffer& buffer)
		{
			Buffer* bufferPtr = &buffer;
			m_bufferQueue.Push(bufferPtr);
		}

		void VoiceSDL2::Play()
		{
			SDL_PauseAudioDevice(m_sdlVoiceId, 0);
			mState = Playing;
		}

		void VoiceSDL2::Stop()
		{
			SDL_PauseAudioDevice(m_sdlVoiceId, 1);
			mState = Stopped;
		}

		void VoiceSDL2::Pause()
		{
			SDL_PauseAudioDevice(m_sdlVoiceId, 1);
			mState = Paused;
		}

		void VoiceSDL2::Resume()
		{
			SDL_PauseAudioDevice(m_sdlVoiceId, 0);
			mState = Playing;
		}

		u64 VoiceSDL2::GetPositionSamples()
		{
			return 0;
		}

		float VoiceSDL2::GetPositionSeconds()
		{
			return (float)GetPositionSamples() / (float)mSource.GetStreamDesc()->GetSampleRate();
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
			if (!m_currentBuffer && !m_bufferQueue.IsEmpty())
			{
				//Pop next buffer
				m_currentBuffer = m_bufferQueue.Pop();
			}

			if (m_currentBuffer)
			{
				int bytesRemaining = len;

				while (bytesRemaining > 0)
				{
					int bytesToCopy = maths::Min(bytesRemaining, (int)m_currentBuffer->GetSize() - m_bufferPos);
					memory::MemCopy(stream, m_currentBuffer->Get(m_bufferPos), bytesToCopy);
					bytesRemaining -= bytesToCopy;
					m_bufferPos += bytesToCopy;

					if (m_bufferPos == m_currentBuffer->GetSize())
					{
						//Reset read head
						m_bufferPos = 0;
						m_currentBuffer = NULL;

						//Request new buffer
						mSource.RequestBuffer(*this);

						if (m_bufferQueue.IsEmpty())
						{
							//Starved
							debug::log << "VoiceSDL2::SDLFillBuffer() - Buffer starved" << debug::end;
							bytesRemaining = 0;
						}
						else
						{
							//Pop next buffer
							m_currentBuffer = m_bufferQueue.Pop();
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
