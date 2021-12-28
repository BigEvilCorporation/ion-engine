///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Event.h
// Date:		8th January 2014
// Authors:		Matt Phillips
// Description:	Threading and synchronisation
///////////////////////////////////////////////////

#pragma once

#include "core/Platform.h"
#include "core/Types.h"

#if defined ION_PLATFORM_WINDOWS
#include "core/platform/windows/thread/EventWindows.h"
#elif defined ION_PLATFORM_LINUX
#include "core/platform/linux/thread/EventLinux.h"
#elif defined ION_PLATFORM_MACOSX
#include "core/platform/macosx/thread/EventMacOSX.h"
#elif defined ION_PLATFORM_DREAMCAST
#include "core/platform/dreamcast/thread/EventDreamcast.h"
#elif defined ION_PLATFORM_SWITCH
#include "core/platform/nx/thread/EventSwitch.h"
#elif defined ION_PLATFORM_ANDROID
#include "core/platform/android/thread/EventAndroid.h"
#endif

namespace ion
{
	namespace thread
	{
		class Event
		{
		public:
			Event();
			~Event();

			void Signal();
			void Wait();

		private:
			EventImpl m_impl;
		};
	}
}
