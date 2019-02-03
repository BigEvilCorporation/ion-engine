
#if defined ION_PLATFORM_WINDOWS
#include "windows/Windows.h"
#elif defined ION_PLATFORM_LINUX
#include "linux/Linux.h"
#elif defined ION_PLATFORM_MACOSX
#include "macosx/MacOSX.h"
#elif defined ION_PLATFORM_DREAMCAST
#include "dreamcast/Dreamcast.h"
#endif

namespace ion
{
	namespace platform
	{
		void Initialise();
		void Shutdown();
	}
}
