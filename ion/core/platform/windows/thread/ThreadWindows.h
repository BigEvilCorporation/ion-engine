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

#pragma once

#include "core/Platform.h"
#include "core/Types.h"

namespace ion
{
	namespace thread
	{
		typedef unsigned long ThreadId;

		class ThreadImpl
		{
		public:
			ThreadImpl(const std::string& name, void* thread);
			virtual ~ThreadImpl();

			void Run();
			void Join();
			void Yield();
			void SetPriority(u32 priority);
			void SetCoreAffinity(u32 affinityMask);

			u32 GetId() const;

		private:
			static unsigned long WINAPI ThreadFunction(void* params);

			ThreadId m_threadId;
			HANDLE m_threadHndl;
			void* m_thread;
			std::string m_name;
		};
	}
}