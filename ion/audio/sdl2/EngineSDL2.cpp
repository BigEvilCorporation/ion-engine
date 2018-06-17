
#include <core/debug/Debug.h>

#include "EngineSDL2.h"
#include "OutputDeviceSDL2.h"
#include "VoiceSDL2.h"

namespace ion
{
	namespace audio
	{
		Engine* Engine::Create()
		{
			return new EngineSDL2();
		}

		EngineSDL2::EngineSDL2()
		{
			//Initialise SDL audio
			int initResult = SDL_InitSubSystem(SDL_INIT_AUDIO);
			if (initResult < 0)
			{
				debug::error << "EngineSDL2::EngineSDL2() - SDL_InitSubSystem() failed with " << initResult << debug::end;
			}

			std::vector<Device*> devices;
			EnumerateDevices(devices);
		}

		EngineSDL2::~EngineSDL2()
		{
		}

		int EngineSDL2::EnumerateDevices(std::vector<Device*>& devices)
		{
			//Temp: just add default output device
			devices.push_back(new OutputDeviceSDL2());

			return devices.size();
		}

		Voice* EngineSDL2::CreateVoice(Source& source, bool loop)
		{
			return new VoiceSDL2(source, loop);
		}

		void EngineSDL2::ReleaseVoice(Voice& voice)
		{
			delete &(VoiceSDL2&)voice;
		}
	}
}