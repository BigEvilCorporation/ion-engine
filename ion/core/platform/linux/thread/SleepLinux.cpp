///////////////////////////////////////////////////
// (c) 2020 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Sleep.cpp
// Date:		27th August 2020
// Authors:		Matt Phillips
// Description:	Threading and synchronisation
///////////////////////////////////////////////////

#include "core/Platform.h"
#include "core/Types.h"

namespace ion
{
	namespace thread
	{
		void Sleep(u32 milliseconds)
		{
			usleep(milliseconds * 1000);
		}
	}
}
