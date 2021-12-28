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

#include "SemaphoreDreamcast.h"
#include "core/debug/Debug.h"

namespace ion
{
	namespace thread
	{
		SemaphoreImpl::SemaphoreImpl(int maxSignalCount)
		{
			ion::debug::Error("Semaphore::Semaphore() - Unsupported on platform");
		}

		SemaphoreImpl::~SemaphoreImpl()
		{

		}

		void SemaphoreImpl::Signal()
		{

		}

		void SemaphoreImpl::Wait()
		{

		}
	}
}
