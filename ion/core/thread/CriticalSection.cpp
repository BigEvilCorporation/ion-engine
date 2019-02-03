///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		CriticalSection.cpp
// Date:		8th January 2014
// Authors:		Matt Phillips
// Description:	Threading and synchronisation
///////////////////////////////////////////////////

#include "core/thread/CriticalSection.h"
#include "core/debug/Debug.h"

namespace ion
{
	namespace thread
	{
		CriticalSection::CriticalSection()
		{
			#if defined ION_PLATFORM_WINDOWS
			InitializeCriticalSection(&m_criticalSectionHndl);
            #elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
            pthread_mutexattr_t attributes;
            pthread_mutexattr_init(&attributes);
            pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
            if (pthread_mutex_init(&m_criticalSectionHndl, &attributes) != 0)
            {
                debug::error << "CriticalSection::CriticalSection() - pthread_mutex_init() failed" << debug::end;
            }
			#endif
		}

		CriticalSection::~CriticalSection()
		{
			#if defined ION_PLATFORM_WINDOWS
			DeleteCriticalSection(&m_criticalSectionHndl);
            #elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
            pthread_mutex_destroy(&m_criticalSectionHndl);
			#endif
		}

		bool CriticalSection::TryBegin()
		{
			#if defined ION_PLATFORM_WINDOWS
			return TryEnterCriticalSection(&m_criticalSectionHndl) != 0;
            #elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
            return pthread_mutex_trylock(&m_criticalSectionHndl) == 0;
			#else
			return false;
			#endif
		}

		void CriticalSection::Begin()
		{
			#if defined ION_PLATFORM_WINDOWS
			EnterCriticalSection(&m_criticalSectionHndl);
            #elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
            pthread_mutex_lock(&m_criticalSectionHndl);
			#endif
		}

		void CriticalSection::End()
		{
			#if defined ION_PLATFORM_WINDOWS
			LeaveCriticalSection(&m_criticalSectionHndl);
            #elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
            pthread_mutex_unlock(&m_criticalSectionHndl);
			#endif
		}
	}
}
