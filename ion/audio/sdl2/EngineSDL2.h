#include <audio/Engine.h>

#include <SDL2/SDL.h>

namespace ion
{
	namespace audio
	{
		class EngineSDL2 : public Engine
		{
			friend class Engine;
		public:
			virtual int EnumerateDevices(std::vector<Device*>& devices);

			virtual Voice* CreateVoice(Source& source, bool loop);
			virtual void ReleaseVoice(Voice& voice);

		protected:
			EngineSDL2();
			virtual ~EngineSDL2();
		};
	}
}