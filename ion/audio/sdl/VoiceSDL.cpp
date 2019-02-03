#include <core/debug/Debug.h>
#include <core/memory/Memory.h>
#include <maths/Maths.h>
#include <audio/Buffer.h>
#include <audio/StreamDesc.h>
#include <audio/Source.h>

#include "VoiceSDL.h"

#define ION_SDL_USE_CALLBACK 0

namespace ion
{
	namespace audio
	{
		VoiceSDL::VoiceSDL(Source& source, bool loop)
			: Voice(source, loop)
		{
			m_startTime = 0;
		}

		VoiceSDL::~VoiceSDL()
		{
		}

		void VoiceSDL::SubmitBuffer(Buffer& buffer)
		{

		}

		void VoiceSDL::Play()
		{
			m_startTime = time::GetSystemTicks();
		}

		void VoiceSDL::Stop()
		{

		}

		void VoiceSDL::Pause()
		{

		}

		void VoiceSDL::Resume()
		{

		}

		u32 VoiceSDL::GetBufferedBytes()
		{
			return 0;
		}

		u64 VoiceSDL::GetPositionSamples()
		{
			return GetPositionSeconds() * (double)mSource.GetStreamDesc()->GetSampleRate();
		}

		double VoiceSDL::GetPositionSeconds()
		{
			return time::TicksToSeconds(time::GetSystemTicks() - m_startTime);
		}

		void VoiceSDL::Update()
		{
		}

		void VoiceSDL::SetVolume(float volume)
		{

		}

		void VoiceSDL::SetPitch(float pitch)
		{

		}
	}
}
