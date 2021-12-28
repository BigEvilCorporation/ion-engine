///////////////////////////////////////////////////
// (c) 2020 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Thread.h
// Date:		27th August 2020
// Authors:		Matt Phillips
// Description:	Threading and synchronisation
///////////////////////////////////////////////////

#include "Thread.h"

namespace ion
{
	namespace thread
	{
		Thread::Thread(const std::string& name)
			: m_impl(name, this)
		{

		}

		Thread::~Thread()
		{

		}

		void Thread::Run()
		{
			m_impl.Run();
		}

		void Thread::Join()
		{
			m_impl.Join();
		}

		void Thread::Yield()
		{
			m_impl.Yield();
		}

		void Thread::SetPriority(Priority priority)
		{
			m_impl.SetPriority((u32)priority);
		}

		void Thread::SetCoreAffinity(u32 affinityMask)
		{
			m_impl.SetCoreAffinity(affinityMask);
		}

		u32 Thread::GetId() const
		{
			return m_impl.GetId();
		}
	}
}