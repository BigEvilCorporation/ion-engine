///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Time.cpp
// Date:		25th July 2011
// Authors:		Matt Phillips
// Description:	System time and ticks
///////////////////////////////////////////////////

#include "Time.h"

#if defined ION_PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace ion
{
	namespace time
	{
		u64 GetSystemTicks()
		{
#if defined ION_PLATFORM_WINDOWS
			LARGE_INTEGER ticks;
			QueryPerformanceCounter(&ticks);
			return (u64)ticks.QuadPart;
#elif defined ION_PLATFORM_DREAMCAST
			return 0;
#endif
		}

		double TicksToSeconds(u64 ticks)
		{
			static double timerFrequency = 0.0;

			if(timerFrequency == 0)
			{
#if defined ION_PLATFORM_WINDOWS
				LARGE_INTEGER freq = {0};
				QueryPerformanceFrequency(&freq);
				timerFrequency = (double)freq.QuadPart;
#endif
			}

			return (double)ticks / timerFrequency;
		}
	}
}