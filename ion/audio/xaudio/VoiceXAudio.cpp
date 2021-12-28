#include <windows.h>
#include <mmreg.h>

#include <core/debug/Debug.h>
#include <audio/Buffer.h>
#include <audio/StreamDesc.h>
#include <audio/Source.h>

#include "VoiceXAudio.h"

namespace ion
{
	namespace audio
	{
		VoiceXAudio::VoiceXAudio(IXAudio2* xaudio2System, Source& source, bool loop)
			: Voice(source, loop)
		{
			m_XAudioVoice = nullptr;
			m_buffersQueued = 0;
			m_bytesBuffered = 0;
			m_bytesConsumed = 0;
			m_samplesPlayed = 0;

			//Get stream desc
			const StreamDesc* streamDesc = source.GetStreamDesc();

			//Create WAVEFORMATEX descriptor
			WAVEFORMATEX waveFormatEx = {0};
			waveFormatEx.wFormatTag = WAVE_FORMAT_PCM;
			waveFormatEx.nChannels = streamDesc->GetNumChannels();
			waveFormatEx.nSamplesPerSec = streamDesc->GetSampleRate();
			waveFormatEx.nAvgBytesPerSec = (streamDesc->GetBitsPerSample() / 8) * streamDesc->GetSampleRate() * streamDesc->GetNumChannels();
			waveFormatEx.nBlockAlign = streamDesc->GetBlockSize();
			waveFormatEx.wBitsPerSample = streamDesc->GetBitsPerSample();
			waveFormatEx.cbSize = 0;

			//Create voice
			HRESULT result = xaudio2System->CreateSourceVoice(&m_XAudioVoice, &waveFormatEx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, this);
			debug::Assert(result == S_OK && m_XAudioVoice, "VoiceXAudio::VoiceXAudio() - Could not create XAudio2 voice");

			//Set default properties
			SetVolume(1.0f);
			SetPitch(1.0f);
		}

		VoiceXAudio::~VoiceXAudio()
		{
		}

		void VoiceXAudio::SubmitBuffer(Buffer& buffer)
		{
			//Lock buffer until finished with
			buffer.ReadLock();

			const StreamDesc* streamDesc = m_source.GetStreamDesc();

			XAUDIO2_BUFFER xaudioBuffer = {0};

			xaudioBuffer.pAudioData = (const BYTE*)buffer.Get(0);
			xaudioBuffer.AudioBytes = buffer.GetDataSize();
			xaudioBuffer.PlayLength = buffer.GetDataSize() / (streamDesc->GetBitsPerSample() / 8) / streamDesc->GetNumChannels();
			xaudioBuffer.pContext = &buffer;
			xaudioBuffer.LoopCount = ((m_source.GetFeedType() == Source::FeedType::SingleBuffer) && m_loop) ? XAUDIO2_LOOP_INFINITE : 0;
			xaudioBuffer.Flags = (m_source.GetFeedType() == Source::FeedType::SingleBuffer) ? XAUDIO2_END_OF_STREAM : 0;

			m_XAudioVoice->SubmitSourceBuffer(&xaudioBuffer);

			ion::thread::atomic::Add(m_bytesBuffered, buffer.GetDataSize());
			ion::thread::atomic::Increment(m_buffersQueued);
		}

		void VoiceXAudio::Play()
		{
			m_XAudioVoice->Start(0);
			m_state = State::Playing;
		}

		void VoiceXAudio::Stop()
		{
			m_XAudioVoice->Stop();
			m_state = State::Stopped;
		}

		void VoiceXAudio::Pause()
		{
			m_XAudioVoice->Stop();
			m_state = State::Paused;
		}

		void VoiceXAudio::Resume()
		{
			m_XAudioVoice->Start();
			m_state = State::Playing;
		}

		u32 VoiceXAudio::GetQueuedBuffers()
		{
			return m_buffersQueued;
		}

		u32 VoiceXAudio::GetBufferedBytes()
		{
			return m_bytesBuffered;
		}

		u32 VoiceXAudio::GetConsumedBytes()
		{
			return m_bytesConsumed;
		}

		u64 VoiceXAudio::GetPositionSamples()
		{
			return m_samplesPlayed;
		}

		double VoiceXAudio::GetPositionSeconds()
		{
			return (double)GetPositionSamples() / (double)m_source.GetStreamDesc()->GetSampleRate();
		}

		void VoiceXAudio::Update(float deltaTime)
		{
			Voice::Update(deltaTime);
		}

		void VoiceXAudio::SetVolume(float volume)
		{
			m_XAudioVoice->SetVolume(volume);
			Voice::SetVolume(volume);
		}

		void VoiceXAudio::SetPitch(float pitch)
		{
			m_XAudioVoice->SetFrequencyRatio(pitch);
			Voice::SetPitch(pitch);
		}

		void VoiceXAudio::OnBufferEnd(void* bufferContext)
		{
			//Done with buffer, count it
			Buffer* buffer = (Buffer*)bufferContext;

			ion::thread::atomic::Add(m_bytesConsumed, buffer->GetDataSize());
			ion::thread::atomic::Decrement(m_buffersQueued);

			//Unlock it
			buffer->ReadUnlock();

			//Get next
			m_source.RequestBuffer(*this);

			if (m_buffersQueued == 0)
			{
				ion::debug::Log("VoiceXAudio::OnBufferEnd() - Voice is starved");
			}
		}

		void VoiceXAudio::OnStreamEnd()
		{
			if (m_source.GetFeedType() == Source::FeedType::SingleBuffer)
			{
				m_state = State::Stopped;
			}
			else
			{
				ion::debug::Log("VoiceXAudio::OnStreamEnd() - voice is starved of data");
			}
		}

		void VoiceXAudio::OnVoiceProcessingPassEnd()
		{
			XAUDIO2_VOICE_STATE state;
			m_XAudioVoice->GetState(&state);
			ion::thread::atomic::Swap(m_samplesPlayed, (u64)state.SamplesPlayed);
		}

		void VoiceXAudio::OnVoiceError(void* bufferContext, HRESULT error)
		{
			ion::debug::error << "VoiceXAudio::OnVoiceError() - error: " << error << ion::debug::end;
		}
	}
}