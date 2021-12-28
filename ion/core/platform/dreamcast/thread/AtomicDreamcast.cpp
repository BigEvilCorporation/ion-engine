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
				ion::debug::Error("ion::thread::atomic::Swap() not implemented on platform");
				return 0;
			}

			u64 Swap(u64& integer, u64 value)
			{
				ion::debug::Error("ion::thread::atomic::Swap() not implemented on platform");
				return 0;
			}

			u32 Add(u32& integer, u32 value)
			{
				ion::debug::Error("ion::thread::atomic::Add() not implemented on platform");
				return 0;
			}

			u64 Add(u64& integer, u64 value)
			{
				ion::debug::Error("ion::thread::atomic::Add() not implemented on platform");
				return 0;
			}

			u32 Increment(u32& integer)
			{
				ion::debug::Error("ion::thread::atomic::Increment() not implemented on platform");
				return 0;
			}

			u64 Increment(u64& integer)
			{
				ion::debug::Error("ion::thread::atomic::Increment() not implemented on platform");
				return 0;
			}

			u32 Decrement(u32& integer)
			{
				ion::debug::Error("ion::thread::atomic::Decrement() not implemented on platform");
				return 0;
			}

			u64 Decrement(u64& integer)
			{
				ion::debug::Error("ion::thread::atomic::Decrement() not implemented on platform");
				return 0;
			}
		}
	}
}
