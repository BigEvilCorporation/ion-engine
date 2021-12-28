///////////////////////////////////////////////////
// File:		UserNull.cpp
// Date:		11th October 2019
// Authors:		Matt Phillips
// Description:	Null user interface
///////////////////////////////////////////////////

#include "UserNull.h"

namespace ion
{
	namespace services
	{
		UserNull::UserNull(const std::string& name)
			: User(name)
		{

		}

		UserNull::~UserNull()
		{

		}
	}
}