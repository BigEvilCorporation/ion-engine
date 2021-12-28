///////////////////////////////////////////////////
// File:		Platform.h
// Date:		26th January 2017
// Authors:		Matt Phillips
// Description:	Platform includes, types, macros
///////////////////////////////////////////////////

#include "Linux.h"
#include "core/debug/CrashHandler.h"

#include <iostream>

namespace ion
{
	namespace platform
	{
		void Initialise()
		{

		}

		void Shutdown()
		{

		}
	}
}

int main(int numargs, char** args)
{
	return ion::EntryPoint(numargs, args);
}