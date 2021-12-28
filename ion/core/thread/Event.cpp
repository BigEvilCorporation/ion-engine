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

#include "Event.h"

namespace ion
{
	namespace thread
	{
		Event::Event()
		{
		}

		Event::~Event()
		{
		}

		void Event::Signal()
		{
			m_impl.Signal();
		}

		void Event::Wait()
		{
			m_impl.Wait();
		}
	}
}