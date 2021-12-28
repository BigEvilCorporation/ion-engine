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

#include "ThreadWindows.h"
#include "core/debug/Debug.h"
#include "core/thread/Thread.h"

namespace ion
{
	namespace thread
	{
		ThreadId GetCurrentThreadId()
		{
			return (ThreadId)::GetCurrentThreadId();
		}

		ThreadImpl::ThreadImpl(const std::string& name, void* thread)
			: m_name(name)
			, m_thread(thread)
		{

		}

		ThreadImpl::~ThreadImpl()
		{
			Join();
			CloseHandle(m_threadHndl);
		}

		void ThreadImpl::Run()
		{
			m_threadHndl = CreateThread(NULL,                   //Default security attributes
										0,                      //Default stack size  
										ThreadImpl::ThreadFunction, //Thread function
										m_thread,					//User data
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
		}

		void ThreadImpl::Join()
		{
			WaitForSingleObject(m_threadHndl, INFINITE);
		}

		void ThreadImpl::Yield()
		{
			SwitchToThread();
		}

		u32 ThreadImpl::GetId() const
		{
			return m_threadId;
		}

		void ThreadImpl::SetPriority(u32 priority)
		{
			int value = 0;

			switch (priority)
			{
			case (u32)Thread::Priority::Low:
				value = THREAD_MODE_BACKGROUND_BEGIN;
				break;
			case (u32)Thread::Priority::Normal:
				value = THREAD_PRIORITY_NORMAL;
				break;
			case (u32)Thread::Priority::High:
				value = THREAD_PRIORITY_HIGHEST;
				break;
			case (u32)Thread::Priority::Critical:
				value = THREAD_PRIORITY_TIME_CRITICAL;
				break;
			}

			if (!SetThreadPriority(m_threadHndl, value))
			{
				debug::error << "Thread::SetPriority() - SetThreadPriority() failed" << debug::end;
			}
		}

		void ThreadImpl::SetCoreAffinity(u32 affinityMask)
		{
			ion::debug::Error("Thread::SetCoreAffinity() - Unsupported on platform");
		}

		unsigned long WINAPI ThreadImpl::ThreadFunction(void* params)
		{
			Thread* thread = (Thread*)params;
			thread->Entry();
			return 0;
		}
	}
}
