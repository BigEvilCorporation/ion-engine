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

#include "CriticalSection.h"

namespace ion
{
	namespace thread
	{
		CriticalSection::CriticalSection()
		{

		}

		CriticalSection::~CriticalSection()
		{

		}

		bool CriticalSection::TryBegin()
		{
			return m_impl.TryBegin();
		}

		void CriticalSection::Begin()
		{
			m_impl.Begin();
		}

		void CriticalSection::End()
		{
			m_impl.End();
		}
	}
}
