///////////////////////////////////////////////////
// File:		PlatformNull.cpp
// Date:		30th July 2018
// Authors:		Matt Phillips
// Description:	Null platform services interface
///////////////////////////////////////////////////

#include "PlatformNull.h"

namespace ion
{
	namespace services
	{
		Platform* Platform::Create(const std::string& appId, const std::string& appEncryptionKey)
		{
			return new PlatformNull(appId, appEncryptionKey);
		}
	}
}