///////////////////////////////////////////////////
// File:		Platform.h
// Date:		26th January 2017
// Authors:		Matt Phillips
// Description:	Platform includes, types, macros
///////////////////////////////////////////////////

#include "MacOSX.h"
#include "core/debug/CrashHandler.h"

#include <iostream>

namespace ion
{
	namespace platform
	{
		void Initialise()
		{
#if defined ION_BUILD_MASTER
			char path[PATH_MAX];
			uint32_t pathLen = sizeof(path);
			int err = _NSGetExecutablePath(path, &pathLen);
			assert(!err);
			chdir(dirname(path));
#endif
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