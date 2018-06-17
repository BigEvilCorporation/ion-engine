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
			{m_name
			}
			#elif defined ION_PLATFORM_LINUX
			int result = pthread_create(&m_threadHndl, NULL, ThreadFunction, this);
			debug::Assert(result == 0, "Thread::Thread() - pthread_create() failed");
            pthread_setname_np(m_threadHndl, m_name.c_str());
			#endif
		}

		void Thread::Join()
		{
			#if defined ION_PLATFORM_WINDOWS
			WaitForSingleObject(m_threadHndl, INFINITE);
			#endif
		}

		u32 Thread::GetId() const
		{
			return m_threadId;
		}

		void Thread::SetPriority(Priority priority)
		{
#if defined ION_PLATFORM_WINDOWS
#elif defined ION_PLATFORM_LINUX
			int result = 0;

			int policy = 0;
			sched_param param;
			pthread_getschedparam(m_threadHndl, &policy, &param);

			int minPrio = sched_get_priority_min(policy);
			int maxPrio = sched_get_priority_max(policy);
			int medPrio = (maxPrio - minPrio) / 2;
			
			switch(priority)
			{
				case Priority::Low:
					result = pthread_setschedprio(m_threadHndl, minPrio);
					break;
				case Priority::Normal:
					result = pthread_setschedprio(m_threadHndl, medPrio);
					break;
				case Priority::High:
					result = pthread_setschedprio(m_threadHndl, maxPrio);
					break;
			}

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
		#elif defined ION_PLATFORM_LINUX
		void* Thread::ThreadFunction(void* params)
		{
			
			Thread* thread = (Thread*)params;
			thread->Entry();
			return NULL;
		}
		#endif
	}
}
