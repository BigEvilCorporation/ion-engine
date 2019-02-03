///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Thread.h
// Date:		25th July 2011
// Authors:		Matt Phillips
// Description:	Threading and synchronisation
///////////////////////////////////////////////////

#pragma once

#include "core/Platform.h"
#include "core/Types.h"

#include <string>

namespace ion
{
	namespace thread
	{
#if defined ION_PLATFORM_WINDOWS
		typedef unsigned long ThreadId;
#else
		typedef u64 ThreadId;
#endif

		ThreadId GetCurrentThreadId();

		class Thread
		{
		public:

			enum class Priority
			{
				Low,
				Normal,
				High,
				Critical
			};

			Thread(const char* name = NULL);
			virtual ~Thread();

			void Run();
			void Join();
			void Yield();
			void SetPriority(Priority priority);
			
			u32 GetId() const;

		protected:
			virtual void Entry() = 0;

		private:
			#if defined ION_PLATFORM_WINDOWS
			static unsigned long WINAPI ThreadFunction(void* params);
			#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
			static void* ThreadFunction(void* params);
			#endif

			ThreadId m_threadId;
			std::string m_name;

			#if defined ION_PLATFORM_WINDOWS
			HANDLE m_threadHndl;
			#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
			pthread_t m_threadHndl;
			#endif
		};
	}
}
