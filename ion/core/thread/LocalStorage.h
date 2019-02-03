///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		LocalStorage.h
// Date:		10th January 2014
// Authors:		Matt Phillips
// Description:	Threading and synchronisation
///////////////////////////////////////////////////

#pragma once

#include "core/Platform.h"
#include "core/Types.h"
#include "core/thread/CriticalSection.h"
#include "core/thread/Atomic.h"
#include <vector>

namespace ion
{
	namespace thread
	{
		template <typename T, int MAX_SIZE> class LocalStorage
		{
		public:
			LocalStorage();
			~LocalStorage();

			//Allocate data for this thread
			bool IsAllocated() const;
			void Allocate();

			//Set/get data for this thread
			void SetData(const T& data);
			T& GetData();

			//Set/get data by object index
			void SetData(const T& data, int index);
			T& GetData(int index);

		private:
			void Store(void* ptr);
			void* Retrieve();

			#if defined ION_PLATFORM_WINDOWS
			DWORD m_TLSContext;
			#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
			pthread_key_t m_TLSContext;
			#endif

			T m_data[MAX_SIZE];
			u32 m_nextIdx;
			CriticalSection m_criticalSection;
		};
	}
}

namespace ion
{
	namespace thread
	{
		template <typename T, int MAX_SIZE> LocalStorage<T, MAX_SIZE>::LocalStorage()
		{
			m_nextIdx = 0;

#if defined ION_PLATFORM_WINDOWS
			m_TLSContext = TlsAlloc();
			debug::Assert(m_TLSContext != TLS_OUT_OF_INDEXES, "Could not create TLS - out of indices");
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
			pthread_key_create(&m_TLSContext, NULL);
#else
			debug::Error("LocalStorage::LocalStorage() - Not implemented on this platform");
#endif
		}

		template <typename T, int MAX_SIZE> LocalStorage<T, MAX_SIZE>::~LocalStorage()
		{
#if defined ION_PLATFORM_WINDOWS
			TlsFree(m_TLSContext);
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
			pthread_key_delete(m_TLSContext);
#endif
		}

		template <typename T, int MAX_SIZE> bool LocalStorage<T, MAX_SIZE>::IsAllocated() const
		{
#if defined ION_PLATFORM_WINDOWS
			return TlsGetValue(m_TLSContext) != nullptr;
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
			return pthread_getspecific(m_TLSContext) != nullptr;
#endif
		}

		template <typename T, int MAX_SIZE> void LocalStorage<T, MAX_SIZE>::Store(void* ptr)
		{
#if defined ION_PLATFORM_WINDOWS
			TlsSetValue(m_TLSContext, ptr);
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
			pthread_setspecific(m_TLSContext, ptr);
#endif
		}

		template <typename T, int MAX_SIZE> void* LocalStorage<T, MAX_SIZE>::Retrieve()
		{
#if defined ION_PLATFORM_WINDOWS
			return TlsGetValue(m_TLSContext);
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
			return pthread_getspecific(m_TLSContext);
#else
			return NULL;
#endif
		}

		template <typename T, int MAX_SIZE> void LocalStorage<T, MAX_SIZE>::Allocate()
		{
			//Get next in array
			m_criticalSection.Begin();
			int index = m_nextIdx;
			atomic::Increment(m_nextIdx);
			m_criticalSection.End();

			//Get array ptr
			T* newObjectPtr = &m_data[index];

			//Store ptr
			Store(newObjectPtr);
		}

		template <typename T, int MAX_SIZE> void LocalStorage<T, MAX_SIZE>::SetData(const T& data)
		{
			//If not yet allocated an object for this thread
			if (!IsAllocated())
			{
				//Allocate for this thread now
				Allocate();
			}

			//Get ptr
			T* ptr = (T*)Retrieve();

			//Set data
			*ptr = data;
		}

		template <typename T, int MAX_SIZE> T& LocalStorage<T, MAX_SIZE>::GetData()
		{
			//Retrieve ptr
			return *(T*)Retrieve();
		}

		template <typename T, int MAX_SIZE> void LocalStorage<T, MAX_SIZE>::SetData(const T& data, int index)
		{
			debug::Assert(index < MAX_SIZE, "LocalStorage<T, S>::SetData() - Index out of bounds");

			m_criticalSection.Begin();
			m_data[index] = data;
			m_criticalSection.End();
		}

		template <typename T, int MAX_SIZE> T& LocalStorage<T, MAX_SIZE>::GetData(int index)
		{
			return m_data[index];
		}
	}
}
