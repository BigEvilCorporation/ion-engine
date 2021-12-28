///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Semaphore.h
// Date:		8th January 2014
// Authors:		Matt Phillips
// Description:	Threading and synchronisation
///////////////////////////////////////////////////

#pragma once

#include "core/Platform.h"
#include "core/Types.h"

#if defined ION_PLATFORM_WINDOWS
#include "core/platform/windows/thread/SemaphoreWindows.h"
#elif defined ION_PLATFORM_LINUX
#include "core/platform/linux/thread/SemaphoreLinux.h"
#elif defined ION_PLATFORM_MACOSX
#include "core/platform/macosx/thread/SemaphoreMacOSX.h"
#elif defined ION_PLATFORM_DREAMCAST
#include "core/platform/dreamcast/thread/SemaphoreDreamcast.h"
#elif defined ION_PLATFORM_SWITCH
#include "core/platform/nx/thread/SemaphoreSwitch.h"
#elif defined ION_PLATFORM_ANDROID
#include "core/platform/android/thread/SemaphoreAndroid.h"
#endif

namespace ion
{
	namespace thread
	{
		class Semaphore
		{
		public:
			Semaphore(int maxSignalCount);
			~Semaphore();

			void Signal();
			void Wait();

		private:
			SemaphoreImpl m_impl;
		};
	}
}
