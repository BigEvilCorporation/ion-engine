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
			dbgio_write_str(message);
			dbgio_write_str("\n");
			dbgio_flush();
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
			while (1) {}
#endif
		}

		void PrintMemoryUsage()
		{
#if !defined ION_BUILD_MASTER
			malloc_stats();
			pvr_mem_stats();

			u32 systemRam = 0x8d000000 - 0x8c000000;
			u32 elfOffset = 0x8c000000;
			u32 stackSize = (int)&_end - (int)&_start + ((int)&_start - elfOffset);

			struct mallinfo mi = mallinfo();
			u32 systemRamFree = systemRam - (mi.usmblks + stackSize);

			printf("Total system RAM: %i (%ikb)\n", systemRam, systemRam / 1024);
			printf("Allocated system RAM: %i (%ikb)\n", mi.usmblks, mi.usmblks / 1024);
			printf("Allocated stack size: %i (%ikb)\n", stackSize, stackSize / 1024);
			printf("Free system RAM: %i (%ikb)\n", systemRamFree, systemRamFree / 1024);
#endif
		}

		u32 GetRAMUsed()
		{
#if !defined ION_BUILD_MASTER
			struct mallinfo mi = mallinfo();
			return mi.uordblks;
#endif
			return 0;
		}
	}
}