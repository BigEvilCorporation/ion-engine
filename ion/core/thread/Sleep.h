///////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Thread.h
// Date:		13th June 2018
// Authors:		Matt Phillips
// Description:	Threading and synchronisation
///////////////////////////////////////////////////

#pragma once

#include "core/Platform.h"
#include "core/Types.h"

namespace ion
{
	namespace thread
	{
		static void Sleep(u32 milliseconds)
		{
			#if defined ION_PLATFORM_WINDOWS
			::Sleep(milliseconds);
            #elif defined ION_PLATFORM_LINUX
            usleep(milliseconds * 1000);
			#endif
		}
	}
}
