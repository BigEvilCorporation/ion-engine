#include <core/debug/Debug.h>
#include <audio/Buffer.h>
#include <audio/StreamDesc.h>
#include <audio/Source.h>

#include "VoiceAndroid.h"

namespace ion
{
	namespace audio
	{
		VoiceAndroid::VoiceAndroid(Source& source, bool loop)
			: Voice(source, loop)
		{

		}

		VoiceAndroid::~VoiceAndroid()
		{
		}

		void VoiceAndroid::SubmitBuffer(Buffer& buffer)
		{

		}

		void VoiceAndroid::Play()
		{

		}

		void VoiceAndroid::Stop()
		{

		}

		void VoiceAndroid::Pause()
		{

		}

		void VoiceAndroid::Resume()
		{

		}

		u32 VoiceAndroid::GetBufferedBytes()
		{
			return 0;
		}

		u64 VoiceAndroid::GetPositionSamples()
		{
			return 0;
		}

		double VoiceAndroid::GetPositionSeconds()
		{
			return 0.0;
		}

		void VoiceAndroid::Update()
		{
		}

		void VoiceAndroid::SetVolume(float volume)
		{

		}

		void VoiceAndroid::SetPitch(float pitch)
		{

		}
	}
}