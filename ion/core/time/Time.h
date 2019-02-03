///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Time.h
// Date:		25th July 2011
// Authors:		Matt Phillips
// Description:	System time and ticks
///////////////////////////////////////////////////

#pragma once

#include "core/Types.h"

namespace ion
{
	namespace time
	{
		struct TimeStamp
		{
			TimeStamp();

			u32 GetSecond();
			u32 GetMinute();
			u32 GetHour();
			u32 GetDay();
			u32 GetMonth();
			u32 GetYear();

			s64 time;
		};

		u64 GetSystemTicks();
		double TicksToSeconds(u64 ticks);

		TimeStamp GetLocalTime();
	}
}