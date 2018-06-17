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
			mXAudioVoice = NULL;

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
			HRESULT result = xaudio2System->CreateSourceVoice(&mXAudioVoice, &waveFormatEx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, this);
			debug::Assert(result == S_OK && mXAudioVoice, "VoiceXAudio::VoiceXAudio() - Could not create XAudio2 voice");

			//Request first buffer
			source.RequestBuffer(*this);

			//Set default properties
			SetVolume(1.0f);
			SetPitch(1.0f);
		}

		VoiceXAudio::~VoiceXAudio()
		{
		}

		void VoiceXAudio::SubmitBuffer(Buffer& buffer)
		{
			buffer.Lock();

			XAUDIO2_BUFFER xaudioBuffer = {0};

			xaudioBuffer.pAudioData = (const BYTE*)buffer.Get(0);
			xaudioBuffer.AudioBytes = buffer.GetDataSize();
			xaudioBuffer.PlayLength = mSource.GetStreamDesc()->GetSizeSamples();

			if (!mLoop)
			{
				xaudioBuffer.Flags = XAUDIO2_END_OF_STREAM;
			}

			mXAudioVoice->SubmitSourceBuffer(&xaudioBuffer);

			buffer.Unlock();
		}

		void VoiceXAudio::Play()
		{
			mXAudioVoice->Start(0);
			mState = Playing;
		}

		void VoiceXAudio::Stop()
		{
			mXAudioVoice->Stop();
			mState = Stopped;
		}

		void VoiceXAudio::Pause()
		{
			mXAudioVoice->Stop();
			mState = Paused;
		}

		void VoiceXAudio::Resume()
		{
			mXAudioVoice->Start();
			mState = Playing;
		}

		u64 VoiceXAudio::GetPositionSamples()
		{
			XAUDIO2_VOICE_STATE state;
			mXAudioVoice->GetState(&state);
			return state.SamplesPlayed;
		}

		float VoiceXAudio::GetPositionSeconds()
		{
			return (float)GetPositionSamples() / (float)mSource.GetStreamDesc()->GetSampleRate();
		}

		void VoiceXAudio::Update()
		{
		}

		void VoiceXAudio::SetVolume(float volume)
		{
			mXAudioVoice->SetVolume(volume);
			Voice::SetVolume(volume);
		}

		void VoiceXAudio::SetPitch(float pitch)
		{
			mXAudioVoice->SetFrequencyRatio(pitch);
			Voice::SetPitch(pitch);
		}

		void VoiceXAudio::OnBufferEnd(void* bufferContext)
		{
			mSource.RequestBuffer(*this);
		}

		void VoiceXAudio::OnStreamEnd()
		{
			mState = Stopped;
		}

		void VoiceXAudio::OnVoiceError(void* bufferContext, HRESULT error)
		{
			ion::debug::error << "VoiceXAudio::OnVoiceError() - error: " << error << ion::debug::end;
		}
	}
}