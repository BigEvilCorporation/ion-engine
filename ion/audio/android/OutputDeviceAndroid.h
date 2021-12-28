#include <audio/OutputDevice.h>

namespace ion
{
	namespace audio
	{
		class OutputDeviceAndroid : public OutputDevice
		{
		public:
			OutputDeviceAndroid();
			virtual ~OutputDeviceAndroid();
		};
	}
}