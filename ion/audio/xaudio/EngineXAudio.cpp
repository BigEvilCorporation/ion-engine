
#include "EngineXAudio.h"
#include "OutputDeviceXAudio.h"
#include "VoiceXAudio.h"
#include <core/debug/Debug.h>

namespace ion
{
	namespace audio
	{
		Engine* Engine::Create()
		{
			return new EngineXAudio();
		}

		EngineXAudio::EngineXAudio()
		{
			//Initialise COM
			CoInitializeEx(NULL, COINIT_MULTITHREADED);

			//Initialise XAudio2
			if(XAudio2Create(&m_XAudio2System, 0, XAUDIO2_DEFAULT_PROCESSOR) != S_OK)
			{
				debug::Error("Could not create XAudio2 system");
			}

			m_XAudio2System->RegisterForCallbacks(this);

#if !defined ION_BUILD_MASTER
			XAUDIO2_DEBUG_CONFIGURATION debugConfig = {0};
			debugConfig.BreakMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
			debugConfig.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS | XAUDIO2_LOG_INFO | XAUDIO2_LOG_DETAIL;
			m_XAudio2System->SetDebugConfiguration(&debugConfig);
#endif

			std::vector<Device*> devices;
			EnumerateDevices(devices);
		}

		EngineXAudio::~EngineXAudio()
		{
		}

		int EngineXAudio::EnumerateDevices(std::vector<Device*>& devices)
		{
			//Temp: just add default output device
			devices.push_back(new OutputDeviceXAudio(*m_XAudio2System));

			return devices.size();
		}

		Voice* EngineXAudio::CreateVoice(Source& source, bool loop)
		{
			Voice* voice = new VoiceXAudio(m_XAudio2System, source, loop);
			AddVoice(voice);
			return voice;
		}

		void EngineXAudio::ReleaseVoice(Voice& voice)
		{
			RemoveVoice(&voice);
			delete &(VoiceXAudio&)voice;
		}

		void EngineXAudio::WaitNextUpdateEvent()
		{
			m_updateEvent.Wait();
		}

		void EngineXAudio::OnProcessingPassStart()
		{

		}

		void EngineXAudio::OnProcessingPassEnd()
		{
			m_updateEvent.Signal();
		}

		void EngineXAudio::OnCriticalError(HRESULT error)
		{
			ion::debug::error << "EngineXAudio::OnCriticalError() - " << error << ion::debug::end;
		}
	}
}