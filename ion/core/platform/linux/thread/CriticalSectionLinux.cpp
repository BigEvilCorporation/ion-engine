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

#include "CriticalSectionLinux.h"
#include "core/debug/Debug.h"

namespace ion
{
	namespace thread
	{
		CriticalSectionImpl::CriticalSectionImpl()
		{
            pthread_mutexattr_t attributes;
            pthread_mutexattr_init(&attributes);
            pthread_mutexattr_settype(&attributes, PTHREAD_MUTEX_RECURSIVE);
            if (pthread_mutex_init(&m_criticalSectionHndl, &attributes) != 0)
            {
                debug::error << "CriticalSection::CriticalSection() - pthread_mutex_init() failed" << debug::end;
            }
		}

		CriticalSectionImpl::~CriticalSectionImpl()
		{
            pthread_mutex_destroy(&m_criticalSectionHndl);
		}

		bool CriticalSectionImpl::TryBegin()
		{
            return pthread_mutex_trylock(&m_criticalSectionHndl) == 0;
		}

		void CriticalSectionImpl::Begin()
		{
            pthread_mutex_lock(&m_criticalSectionHndl);
		}

		void CriticalSectionImpl::End()
		{
            pthread_mutex_unlock(&m_criticalSectionHndl);
		}
	}
}
