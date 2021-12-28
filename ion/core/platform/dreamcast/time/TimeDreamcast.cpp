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

namespace ion
{
	namespace time
	{
		u64 GetSystemTicks()
		{
			return timer_us_gettime64();
		}

		double TicksToSeconds(u64 ticks)
		{
			return (double)ticks / 1000000.0f;
		}
	}
}