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

#include "core/Platform.h"
#include "core/debug/Debug.h"

#include <chrono>

namespace ion
{
	namespace time
	{
		u64 GetSystemTicks()
		{
#if defined ION_PLATFORM_WINDOWS
			LARGE_INTEGER ticks;
			if (!QueryPerformanceCounter(&ticks))
			{
				debug::Error("ion::time::GetSystemTicks() - QueryPerformanceCounter() failed");
			}
			return (u64)ticks.QuadPart;
#elif defined ION_PLATFORM_MACOSX
			timeval time;
			gettimeofday(&time, NULL);
			return (time.tv_sec * 1000000) + time.tv_usec;
#elif defined ION_PLATFORM_LINUX
			timeval time;
			gettimeofday(&time, NULL);
			return (time.tv_sec * 1000000) + time.tv_usec;
#elif defined ION_PLATFORM_RASPBERRYPI
			timeval time;
			gettimeofday(&time, NULL);
			return (time.tv_sec * 1000000) + time.tv_usec;
#elif defined ION_PLATFORM_DREAMCAST
			return timer_us_gettime64();
#endif
		}

		double TicksToSeconds(u64 ticks)
		{
#if defined ION_PLATFORM_WINDOWS
			static double timerFrequency = 0.0;

			if(timerFrequency == 0)
			{
				LARGE_INTEGER freq = {0};
				QueryPerformanceFrequency(&freq);
				timerFrequency = (double)freq.QuadPart;
			}

			return (double)ticks / timerFrequency;
#elif defined ION_PLATFORM_MACOSX
			return (double)ticks / 1000000.0f;
#elif defined ION_PLATFORM_LINUX
			return (double)ticks / 1000000.0f;
#elif defined ION_PLATFORM_RASPBERRYPI
			return (double)ticks / 1000000.0f;
#elif defined ION_PLATFORM_DREAMCAST
			return (double)ticks / 1000000.0f;
#endif
		}

		TimeStamp::TimeStamp()
		{
			time = 0;
		}

		TimeStamp GetLocalTime()
		{
			TimeStamp timeStamp;
			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			timeStamp.time = std::chrono::system_clock::to_time_t(now);
			return timeStamp;
		}

		u32 TimeStamp::GetSecond()
		{
			time_t nativetime = (time_t)time;
			tm localTime = *localtime(&nativetime);
			return localTime.tm_sec;
		}

		u32 TimeStamp::GetMinute()
		{
			time_t nativetime = (time_t)time;
			tm localTime = *localtime(&nativetime);
			return localTime.tm_min;
		}

		u32 TimeStamp::GetHour()
		{
			time_t nativetime = (time_t)time;
			tm localTime = *localtime(&nativetime);
			return localTime.tm_hour;
		}

		u32 TimeStamp::GetDay()
		{
			time_t nativetime = (time_t)time;
			tm localTime = *localtime(&nativetime);
			return localTime.tm_mday;
		}

		u32 TimeStamp::GetMonth()
		{
			time_t nativetime = (time_t)time;
			tm localTime = *localtime(&nativetime);
			return localTime.tm_mon;
		}

		u32 TimeStamp::GetYear()
		{
			time_t nativetime = (time_t)time;
			tm localTime = *localtime(&nativetime);
			return localTime.tm_year + 1900;
		}
	}
}
