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

#if defined ION_PLATFORM_WINDOWS
#include <memory.h>
#elif defined ION_PLATFORM_MACOSX
#include <string.h>
#elif defined ION_PLATFORM_LINUX
#include <memory.h>
#elif defined ION_PLATFORM_RASPBERRYPI
#include <memory.h>
#elif defined ION_PLATFORM_DREAMCAST
#include <memory>
#endif

namespace ion
{
	namespace memory
	{
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
