///////////////////////////////////////////////////
// File:		UserNull.h
// Date:		11th October 2019
// Authors:		Matt Phillips
// Description:	Null user interface
///////////////////////////////////////////////////

#pragma once

#include <services/User.h>

namespace ion
{
	namespace services
	{
		class UserNull : public User
		{
		public:
			UserNull(const std::string& name);
			virtual ~UserNull();
		};
	}
}