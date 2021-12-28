///////////////////////////////////////////////////
// File:		PlatformNull.h
// Date:		30th July 2018
// Authors:		Matt Phillips
// Description:	Steam platform services interface
///////////////////////////////////////////////////

#pragma once

#include <services/Platform.h>

namespace ion
{
	namespace services
	{
		class PlatformNull : public Platform
		{
		public:
			PlatformNull(const std::string& appId, const std::string& appEncryptionKey) {}
			~PlatformNull() {}

		private:
		};
	}
}
