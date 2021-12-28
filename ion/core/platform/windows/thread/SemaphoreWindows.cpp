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

#include "SemaphoreWindows.h"

namespace ion
{
	namespace thread
	{
		SemaphoreImpl::SemaphoreImpl(int maxSignalCount)
		{
			m_semaphoreHndl = CreateSemaphore(NULL, 0, maxSignalCount, NULL);
		}

		SemaphoreImpl::~SemaphoreImpl()
		{
			CloseHandle(m_semaphoreHndl);
		}

		void SemaphoreImpl::Signal()
		{
			ReleaseSemaphore(m_semaphoreHndl, 1, NULL);
		}

		void SemaphoreImpl::Wait()
		{
			WaitForSingleObject(m_semaphoreHndl, INFINITE);
		}
	}
}
