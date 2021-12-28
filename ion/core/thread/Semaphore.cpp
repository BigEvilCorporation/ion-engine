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

#include "Semaphore.h"

namespace ion
{
	namespace thread
	{
		Semaphore::Semaphore(int maxSignalCount)
			: m_impl(maxSignalCount)
		{

		}

		Semaphore::~Semaphore()
		{

		}

		void Semaphore::Signal()
		{
			m_impl.Signal();
		}

		void Semaphore::Wait()
		{
			m_impl.Wait();
		}
	}
}
