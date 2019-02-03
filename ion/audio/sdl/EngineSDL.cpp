
#include <core/debug/Debug.h>

#include "EngineSDL.h"
#include "OutputDeviceSDL.h"
#include "VoiceSDL.h"

namespace ion
{
	namespace audio
	{
		Engine* Engine::Create()
		{
			return new EngineSDL();
		}

		EngineSDL::EngineSDL()
		{

		}

		EngineSDL::~EngineSDL()
		{
		}

		int EngineSDL::EnumerateDevices(std::vector<Device*>& devices)
		{
			return 0;
		}

		Voice* EngineSDL::CreateVoice(Source& source, bool loop)
		{
			return new VoiceSDL(source, loop);
		}

		void EngineSDL::ReleaseVoice(Voice& voice)
		{
			delete &(VoiceSDL&)voice;
		}
	}
}