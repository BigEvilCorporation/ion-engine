///////////////////////////////////////////////////
// File:		Switch.cpp
// Date:		2nd September 2019
// Authors:		Matt Phillips
// Description:	Platform includes, types, macros
///////////////////////////////////////////////////

#include "Android.h"
#include "core/debug/Debug.h"
#include "core/debug/CrashHandler.h"

#include <iostream>

namespace ion
{
	namespace platform
	{
		void Initialise()
		{
			debug::Log("ion::engine for Android");
		}

		void Shutdown()
		{

		}
	}

	namespace debug
	{
		void Log(const char* message)
		{
#if !defined ION_BUILD_MASTER
			std::cout << message << "\n";
#endif
		}

		void Flush()
		{
#if !defined ION_BUILD_MASTER
			std::cout << std::flush;
#endif
		}

		void Error(const char* message)
		{
#if !defined ION_BUILD_MASTER
			Log(message);
			Flush();
			PrintCallstack();
			Break();
#endif
		}

		void Popup(const char* message, const char* title)
		{

		}

		void Break()
		{
#if !defined ION_BUILD_MASTER
			raise(SIGTRAP);
#endif
		}

		void PrintMemoryUsage()
		{
#if !defined ION_BUILD_MASTER
			printf("ion::debug::PrintMemoryUsage() - TODO\n");
#endif
		}

		u32 GetRAMUsed()
		{
			return 0;
		}

		void InstallDefaultCrashHandler()
		{
		}

		void PrintCallstack()
		{
		}
	}
}

int main(int numargs, char** args)
{
	return ion::EntryPoint(numargs, args);
}