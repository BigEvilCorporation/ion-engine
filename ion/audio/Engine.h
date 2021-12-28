#pragma once

#include <core/thread/CriticalSection.h>
#include <core/thread/Event.h>
#include <vector>

namespace ion
{
	namespace audio
	{
		class Device;
		class Source;
		class Voice;

		class Engine
		{
		public:
			static Engine* Create();
			virtual ~Engine() {}

			virtual int EnumerateDevices(std::vector<Device*>& devices) = 0;

			virtual Voice* CreateVoice(Source& source, bool loop) = 0;
			virtual void ReleaseVoice(Voice& voice) = 0;

			virtual void Update(float deltaTime);

			virtual void WaitNextUpdateEvent() {}

		protected:
			Engine() {}
			void AddVoice(Voice* voice);
			void RemoveVoice(Voice* voice);

		private:
			std::vector<Voice*> m_voices;
			ion::thread::CriticalSection m_voiceListCritSec;
		};
	}
}