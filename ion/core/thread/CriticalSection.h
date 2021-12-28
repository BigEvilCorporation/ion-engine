///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		CriticalSection.h
// Date:		8th January 2014
// Authors:		Matt Phillips
// Description:	Threading and synchronisation
///////////////////////////////////////////////////

#pragma once

#include "core/Platform.h"
#include "core/Types.h"

#if defined ION_PLATFORM_WINDOWS
#include "core/platform/windows/thread/CriticalSectionWindows.h"
#elif defined ION_PLATFORM_LINUX
#include "core/platform/linux/thread/CriticalSectionLinux.h"
#elif defined ION_PLATFORM_MACOSX
#include "core/platform/macosx/thread/CriticalSectionMacOSX.h"
#elif defined ION_PLATFORM_DREAMCAST
#include "core/platform/dreamcast/thread/CriticalSectionDreamcast.h"
#elif defined ION_PLATFORM_SWITCH
#include "core/platform/nx/thread/CriticalSectionSwitch.h"
#elif defined ION_PLATFORM_ANDROID
#include "core/platform/android/thread/CriticalSectionAndroid.h"
#endif

namespace ion
{
	namespace thread
	{
		class CriticalSection
		{
		public:
			CriticalSection();
			~CriticalSection();

			bool TryBegin();
			void Begin();
			void End();
			
		private:
			CriticalSectionImpl m_impl;
		};
	}
}
