///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		MemoryManager.h
// Date:		2nd August 2011
// Authors:		Matt Phillips
// Description:	Memory management
///////////////////////////////////////////////////

#pragma once

#include "core/Types.h"

namespace ion
{
	namespace memory
	{
		u8* AllocAligned(u32 alignment, u64 size);
		void FreeAligned(u8* ptr);

		void MemSet(void* address, int value, u32 size);
		void MemCopy(void* destination, const void* source, u32 size);
	}
}
