#include "EngineAndroid.h"
#include "OutputDeviceAndroid.h"
#include "VoiceAndroid.h"
#include <core/debug/Debug.h>

namespace ion
{
	namespace audio
	{
		Engine* Engine::Create()
		{
			return new EngineAndroid();
		}

		EngineAndroid::EngineAndroid()
		{

		}

		EngineAndroid::~EngineAndroid()
		{
		}

		int EngineAndroid::EnumerateDevices(std::vector<Device*>& devices)
		{
			return 0;
		}

		Voice* EngineAndroid::CreateVoice(Source& source, bool loop)
		{
			return new VoiceAndroid(source, loop);
		}

		void EngineAndroid::ReleaseVoice(Voice& voice)
		{
			delete &(VoiceAndroid&)voice;
		}
	}
}