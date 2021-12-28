
#include <audio/Engine.h>

#include <XAudio2.h>

namespace ion
{
	namespace audio
	{
		class EngineXAudio : public Engine, public IXAudio2EngineCallback
		{
			friend class Engine;
		public:
			virtual int EnumerateDevices(std::vector<Device*>& devices);

			virtual Voice* CreateVoice(Source& source, bool loop);
			virtual void ReleaseVoice(Voice& voice);

			virtual void WaitNextUpdateEvent();

		protected:
			EngineXAudio();
			virtual ~EngineXAudio();

			virtual void __stdcall OnProcessingPassStart();
			virtual void __stdcall OnProcessingPassEnd();
			virtual void __stdcall OnCriticalError(HRESULT error);

		private:
			IXAudio2* m_XAudio2System;
			ion::thread::Event m_updateEvent;
		};
	}
}