///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Semaphore.cpp
// Date:		8th January 2014
// Authors:		Matt Phillips
// Description:	Threading and synchronisation
///////////////////////////////////////////////////

#include "core/thread/Semaphore.h"
#include "core/debug/Debug.h"

namespace ion
{
	namespace thread
	{
		Semaphore::Semaphore(int maxSignalCount)
		{
			#if defined ION_PLATFORM_WINDOWS
			m_semaphoreHndl = CreateSemaphore(NULL, 0, maxSignalCount, NULL);
            #elif defined ION_PLATFORM_LINUX
            if(sem_init(&m_semaphoreHndl, 0, 0) != 0)
            {
                debug::error << "Semaphore::Semaphore() - sem_init() failed" << debug::end;
            }
            #elif defined ION_PLATFORM_MACOSX
            m_semaphoreHndl = dispatch_semaphore_create(0);
			#endif
		}

		Semaphore::~Semaphore()
		{
			#if defined ION_PLATFORM_WINDOWS
			CloseHandle(m_semaphoreHndl);
            #elif defined ION_PLATFORM_LINUX
            sem_destroy(&m_semaphoreHndl);
            #elif defined ION_PLATFORM_MACOSX
            dispatch_release(m_semaphoreHndl);
			#endif
		}

		void Semaphore::Signal()
		{
			#if defined ION_PLATFORM_WINDOWS
			ReleaseSemaphore(m_semaphoreHndl, 1, NULL);
            #elif defined ION_PLATFORM_LINUX
            sem_post(&m_semaphoreHndl);
            #elif defined ION_PLATFORM_MACOSX
            dispatch_semaphore_signal(m_semaphoreHndl);
			#endif
		}

		void Semaphore::Wait()
		{
			#if defined ION_PLATFORM_WINDOWS
			WaitForSingleObject(m_semaphoreHndl, INFINITE);
            #elif defined ION_PLATFORM_LINUX
            sem_wait(&m_semaphoreHndl);
            #elif defined ION_PLATFORM_MACOSX
            dispatch_semaphore_wait(m_semaphoreHndl, DISPATCH_TIME_FOREVER);
			#endif
		}
	}
}
