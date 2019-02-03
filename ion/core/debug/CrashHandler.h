///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		CrashHandler.h
// Date:		16th August 2018
// Authors:		Matt Phillips
// Description:	Crash dump handler
///////////////////////////////////////////////////

#pragma once

#include "core/Types.h"

namespace ion
{
	namespace debug
	{
		void InstallDefaultCrashHandler();
		void PrintCallstack();
	}
}