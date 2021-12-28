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

#include "SemaphoreMacOSX.h"
#include "core/debug/Debug.h"

namespace ion
{
	namespace thread
	{
		SemaphoreImpl::SemaphoreImpl(int maxSignalCount)
		{
            m_semaphoreHndl = dispatch_semaphore_create(0);
		}

		SemaphoreImpl::~SemaphoreImpl()
		{
            dispatch_release(m_semaphoreHndl);
		}

		void SemaphoreImpl::Signal()
		{
            dispatch_semaphore_signal(m_semaphoreHndl);
		}

		void SemaphoreImpl::Wait()
		{
            dispatch_semaphore_wait(m_semaphoreHndl, DISPATCH_TIME_FOREVER);
		}
	}
}
