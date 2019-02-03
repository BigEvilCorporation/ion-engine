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
			#if defined ION_PLATFORM_WINDOWS
			return (ThreadId)::GetCurrentThreadId();
			#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
			return (ThreadId)pthread_self();
			#else
			return 0;
			#endif
		}

		Thread::Thread(const char* name)
		{
			if(!name)
			{
				name = "ion::thread";
			}
			
			m_name = name;
		}

		Thread::~Thread()
		{
			#if defined ION_PLATFORM_WINDOWS
			CloseHandle(m_threadHndl);
			#endif
		}

		void Thread::Run()
		{
#if defined ION_PLATFORM_WINDOWS
			m_threadHndl = CreateThread(NULL,                   //Default security attributes
										0,                      //Default stack size  
										Thread::ThreadFunction, //Thread function
										this,					//User data
										0,                      //Default creation flags 
										&m_threadId);			//Thread id

			//Set thread name by invoking VC exception
			const DWORD MS_VC_EXCEPTION = 0x406D1388;

			struct THREADNAME_INFO
			{
				DWORD dwType;
				LPCSTR szName;
				DWORD dwThreadID;
				DWORD dwFlags;
			};

			THREADNAME_INFO nameInfo;
			nameInfo.dwType = 0x1000;
			nameInfo.szName = m_name.c_str();
			nameInfo.dwThreadID = m_threadId;
			nameInfo.dwFlags = 0;

			__try
			{
				RaiseException(MS_VC_EXCEPTION, 0, sizeof(nameInfo)/sizeof(DWORD), (ULONG_PTR*)&nameInfo);
			}
			__except(EXCEPTION_CONTINUE_EXECUTION)
			{
			}
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
			int result = pthread_create(&m_threadHndl, NULL, ThreadFunction, this);
			debug::Assert(result == 0, "Thread::Thread() - pthread_create() failed");
#endif
            
#if defined ION_PLATFORM_LINUX
            pthread_setname_np(m_threadHndl, m_name.c_str());
#endif
		}

		void Thread::Join()
		{
#if defined ION_PLATFORM_WINDOWS
			WaitForSingleObject(m_threadHndl, INFINITE);
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
			pthread_join(m_threadHndl, nullptr);
#endif
		}

		void Thread::Yield()
		{
#if defined ION_PLATFORM_WINDOWS
			SwitchToThread();
#endif
		}

		u32 Thread::GetId() const
		{
			return m_threadId;
		}

		void Thread::SetPriority(Priority priority)
		{
#if defined ION_PLATFORM_WINDOWS
			int value = 0;

			switch (priority)
			{
			case Priority::Low:
				value = THREAD_MODE_BACKGROUND_BEGIN;
				break;
			case Priority::Normal:
				value = THREAD_PRIORITY_NORMAL;
				break;
			case Priority::High:
				value = THREAD_PRIORITY_HIGHEST;
				break;
			case Priority::Critical:
				value = THREAD_PRIORITY_TIME_CRITICAL;
				break;
			}

			if (!SetThreadPriority(m_threadHndl, value))
			{
				debug::error << "Thread::SetPriority() - SetThreadPriority() failed" << debug::end;
			}

#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX
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
#endif
		}

#if defined ION_PLATFORM_WINDOWS
		unsigned long WINAPI Thread::ThreadFunction(void* params)
		{
			
			Thread* thread = (Thread*)params;
			thread->Entry();
			return 0;
		}
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
		void* Thread::ThreadFunction(void* params)
		{
			Thread* thread = (Thread*)params;
#if defined ION_PLATFORM_MACOSX
            pthread_setname_np(thread->m_name.c_str());
#endif
			thread->Entry();
			return NULL;
		}
#endif
	}
}
