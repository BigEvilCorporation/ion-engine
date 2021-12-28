///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Event.cpp
// Date:		8th January 2014
// Authors:		Matt Phillips
// Description:	Threading and synchronisation
///////////////////////////////////////////////////

#include "EventWindows.h"

namespace ion
{
	namespace thread
	{
		EventImpl::EventImpl()
		{
			m_threadEventHndl = CreateEvent(NULL, FALSE, FALSE, NULL);
		}

		EventImpl::~EventImpl()
		{
			CloseHandle(m_threadEventHndl);
		}

		void EventImpl::Signal()
		{
			SetEvent(m_threadEventHndl);
		}

		void EventImpl::Wait()
		{
			WaitForSingleObject(m_threadEventHndl, INFINITE);
		}
	}
}
