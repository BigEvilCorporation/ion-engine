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

#include "SemaphoreAndroid.h"
#include "core/debug/Debug.h"

namespace ion
{
	namespace thread
	{
		SemaphoreImpl::SemaphoreImpl(int maxSignalCount)
		{
            if(sem_init(&m_semaphoreHndl, 0, 0) != 0)
            {
                debug::error << "Semaphore::Semaphore() - sem_init() failed" << debug::end;
            }
		}

		SemaphoreImpl::~SemaphoreImpl()
		{
            sem_destroy(&m_semaphoreHndl);
		}

		void SemaphoreImpl::Signal()
		{
            sem_post(&m_semaphoreHndl);
		}

		void SemaphoreImpl::Wait()
		{
            sem_wait(&m_semaphoreHndl);
		}
	}
}
