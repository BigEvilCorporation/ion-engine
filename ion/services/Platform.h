///////////////////////////////////////////////////
// File:		Services.h
// Date:		11th October 2019
// Authors:		Matt Phillips
// Description:	Platform services interface
///////////////////////////////////////////////////

#pragma once

#include <string>

namespace ion
{
	namespace services
	{
		class Platform
		{
		public:
			static Platform* Create(const std::string& appId, const std::string& appEncryptionKey);

			virtual ~Platform() {}

			virtual void Update() {}
		};
	}
}