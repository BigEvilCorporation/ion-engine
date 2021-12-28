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
			pthread_setname_np(m_threadHndl, m_name.c_str());
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
			int policy = 0;
			sched_param param;
			pthread_getschedparam(m_threadHndl, &policy, &param);

			int minPrio = sched_get_priority_min(policy);
			int maxPrio = sched_get_priority_max(policy);
			int medPrio = (maxPrio - minPrio) / 2;
			int highPrio = medPrio + ((medPrio - maxPrio) / 2);
			
			switch(priority)
			{
				case Priority::Low:
					param.sched_priority = minPrio;
					break;
				case Priority::Normal:
					param.sched_priority = medPrio;
					break;
				case Priority::High:
                    param.sched_priority = highPrio;
					break;
				case Priority::Critical:
					param.sched_priority = maxPrio;
					break;
			}
            
            int result = pthread_setschedparam(m_threadHndl, policy, &param);

			if(result != 0)
			{
				debug::error << "Thread::SetPriority() - pthread_setschedprio() failed" << debug::end;
			}
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
