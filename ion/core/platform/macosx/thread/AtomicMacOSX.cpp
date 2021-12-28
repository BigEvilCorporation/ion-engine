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
			u32 Add(u32& integer, u32 value)
			{
				return __atomic_fetch_add(&integer, value, __ATOMIC_SEQ_CST);
			}

			u64 Add(u64& integer, u64 value)
			{
				return __atomic_fetch_add(&integer, value, __ATOMIC_SEQ_CST);
			}

			void Increment(u32& integer)
			{
				__atomic_fetch_add(&integer, 1, __ATOMIC_SEQ_CST);
			}

			void Increment(u64& integer)
			{
				__atomic_fetch_add(&integer, 1, __ATOMIC_SEQ_CST);
			}

			void Decrement(u32& integer)
			{
				__atomic_fetch_sub(&integer, 1, __ATOMIC_SEQ_CST);
			}

			void Decrement(u64& integer)
			{
				__atomic_fetch_sub(&integer, 1, __ATOMIC_SEQ_CST);
			}
		}
	}
}
