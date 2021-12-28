///////////////////////////////////////////////////
// (c) 2020 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Time.cpp
// Date:		27th August 2020
// Authors:		Matt Phillips
// Description:	System time and ticks
///////////////////////////////////////////////////

#include "core/time/Time.h"
#include "core/Platform.h"
#include "core/debug/Debug.h"

namespace ion
{
	namespace time
	{
		u64 GetSystemTicks()
		{
			LARGE_INTEGER ticks;
			if (!QueryPerformanceCounter(&ticks))
			{
				debug::Error("ion::time::GetSystemTicks() - QueryPerformanceCounter() failed");
			}
			return (u64)ticks.QuadPart;
		}

		double TicksToSeconds(u64 ticks)
		{
			static double timerFrequency = 0.0;

			if (timerFrequency == 0)
			{
				LARGE_INTEGER freq = { 0 };
				QueryPerformanceFrequency(&freq);
				timerFrequency = (double)freq.QuadPart;
			}

			return (double)ticks / timerFrequency;
		}
	}
}