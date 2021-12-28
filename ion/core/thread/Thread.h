///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Thread.h
// Date:		25th July 2011
// Authors:		Matt Phillips
// Description:	Threading and synchronisation
///////////////////////////////////////////////////

#pragma once

#include "core/Platform.h"
#include "core/Types.h"

#include <string>

#if defined ION_PLATFORM_WINDOWS
#include "core/platform/windows/thread/ThreadWindows.h"
#elif defined ION_PLATFORM_LINUX
#include "core/platform/linux/thread/ThreadLinux.h"
#elif defined ION_PLATFORM_MACOSX
#include "core/platform/macosx/thread/ThreadMacOSX.h"
#elif defined ION_PLATFORM_DREAMCAST
#include "core/platform/dreamcast/thread/ThreadDreamcast.h"
#elif defined ION_PLATFORM_SWITCH
#include "core/platform/nx/thread/ThreadSwitch.h"
#elif defined ION_PLATFORM_ANDROID
#include "core/platform/android/thread/ThreadAndroid.h"
#endif

namespace ion
{
	namespace thread
	{
		ThreadId GetCurrentThreadId();

		class Thread
		{
		public:
			enum class Priority
			{
				Low,
				Normal,
				High,
				Critical
			};

			Thread(const std::string& name);
			virtual ~Thread();

			void Run();
			void Join();
			void Yield();
			void SetPriority(Priority priority);
			void SetCoreAffinity(u32 affinityMask);
			u32 GetId() const;

		protected:
			virtual void Entry() = 0;

		private:
			friend class ThreadImpl;
			ThreadImpl m_impl;
		};
	}
}
