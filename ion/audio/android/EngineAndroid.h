#include <audio/Engine.h>

namespace ion
{
	namespace audio
	{
		class EngineAndroid : public Engine
		{
			friend class Engine;
		public:
			virtual int EnumerateDevices(std::vector<Device*>& devices);

			virtual Voice* CreateVoice(Source& source, bool loop);
			virtual void ReleaseVoice(Voice& voice);

		protected:
			EngineAndroid();
			virtual ~EngineAndroid();
		};
	}
}