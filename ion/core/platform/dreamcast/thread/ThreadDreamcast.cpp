///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Thread.cpp
// Date:		25th July 2011
// Authors:		Matt Phillips
// Description:	Threading and synchronisation
///////////////////////////////////////////////////

#include "core/thread/Thread.h"
#include "core/debug/Debug.h"

namespace ion
{
	namespace thread
	{
		ThreadId GetCurrentThreadId()
		{
			return (ThreadId)pthread_self();
		}

		ThreadImpl::ThreadImpl(const std::string& name, void* thread)
			: m_name(name)
			, m_thread(thread)
		{
		}

		Thread::~Thread()
		{
			Join();
		}

		void Thread::Run()
		{
			int result = pthread_create(&m_threadHndl, NULL, ThreadFunction, m_thread);
			debug::Assert(result == 0, "Thread::Thread() - pthread_create() failed");
		}

		void Thread::Join()
		{
			pthread_join(m_threadHndl, nullptr);
		}

		void Thread::Yield()
		{
			ion::debug::Error("Thread::Yield() - Unsupported on platform");
		}

		u32 Thread::GetId() const
		{
			return m_threadId;
		}

		void Thread::SetPriority(Priority priority)
		{
			ion::debug::Error("Thread::SetPriority() - Unsupported on platform");
		}

		void Thread::SetCoreAffinity(u32 affinityMask)
		{
			ion::debug::Error("Thread::SetCoreAffinity() - Unsupported on platform");
		}

		void* Thread::ThreadFunction(void* params)
		{
			Thread* thread = (Thread*)params;
			thread->Entry();
			return NULL;
		}
	}
}
