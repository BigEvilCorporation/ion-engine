///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		MemoryManager.cpp
// Date:		2nd August 2011
// Authors:		Matt Phillips
// Description:	Memory management
///////////////////////////////////////////////////

#include "core/memory/Memory.h"
#include "core/Platform.h"

namespace ion
{
	namespace memory
	{
		u8* AllocAligned(u32 alignment, u64 size)
		{
			return (u8*)_aligned_malloc((size_t)alignment, (size_t)size);
		}

		void FreeAligned(u8* ptr)
		{
			_aligned_free(ptr);
		}

		void MemSet(void* address, int value, u32 size)
		{
			memset(address, value, size);
		}

		void MemCopy(void* destination, const void* source, u32 size)
		{
			memcpy(destination, source, size);
		}
	}
}