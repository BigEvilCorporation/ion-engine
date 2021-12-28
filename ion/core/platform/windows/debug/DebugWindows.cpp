///////////////////////////////////////////////////
// (c) 2020 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Debug.cpp
// Date:		27th August 2020
// Authors:		Matt Phillips
// Description:	Debug output, log
///////////////////////////////////////////////////

#include "core/Platform.h"
#include "core/debug/Debug.h"
#include "core/debug/CrashHandler.h"

#include <iostream>

namespace ion
{
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
#if !defined ION_BUILD_MASTER
			::MessageBoxA(NULL, message, title, MB_OK);
#endif
		}

		void Break()
		{
#if !defined ION_BUILD_MASTER
			__debugbreak();
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
	}
}