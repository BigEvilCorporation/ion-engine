///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Atomic.cpp
// Date:		8th January 2014
// Authors:		Matt Phillips
// Description:	Threading and synchronisation
///////////////////////////////////////////////////

#include "core/thread/Atomic.h"

namespace ion
{
	namespace thread
	{
		namespace atomic
		{
			u32 Swap(u32& integer, u32 value)
			{
				return InterlockedExchange((LONG*)&integer, value);
			}

			u64 Swap(u64& integer, u64 value)
			{
				return InterlockedExchange64((LONG64*)&integer, value);
			}

			u32 Add(u32& integer, u32 value)
			{
				return InterlockedAdd((LONG*)&integer, value);
			}

			u64 Add(u64& integer, u64 value)
			{
				return InterlockedAdd64((LONG64*)&integer, value);
			}

			u32 Increment(u32& integer)
			{
				return InterlockedIncrement((LONG*)&integer);
			}

			u64 Increment(u64& integer)
			{
				return InterlockedIncrement64((LONG64*)&integer);
			}

			u32 Decrement(u32& integer)
			{
				return InterlockedDecrement((LONG*)&integer);
			}

			u64 Decrement(u64& integer)
			{
				return InterlockedDecrement64((LONG64*)&integer);
			}
		}
	}
}
