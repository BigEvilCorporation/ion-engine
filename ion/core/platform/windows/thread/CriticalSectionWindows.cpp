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

#include "CriticalSectionWindows.h"
#include "core/debug/Debug.h"

namespace ion
{
	namespace thread
	{
		CriticalSectionImpl::CriticalSectionImpl()
		{
			InitializeCriticalSection(&m_criticalSectionHndl);
		}

		CriticalSectionImpl::~CriticalSectionImpl()
		{
			DeleteCriticalSection(&m_criticalSectionHndl);
		}

		bool CriticalSectionImpl::TryBegin()
		{
			return TryEnterCriticalSection(&m_criticalSectionHndl) != 0;
		}

		void CriticalSectionImpl::Begin()
		{
			EnterCriticalSection(&m_criticalSectionHndl);
		}

		void CriticalSectionImpl::End()
		{
			LeaveCriticalSection(&m_criticalSectionHndl);
		}
	}
}
