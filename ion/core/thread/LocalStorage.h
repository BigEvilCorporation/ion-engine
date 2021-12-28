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
#include "core/debug/Debug.h"
#include "core/thread/CriticalSection.h"
#include "core/thread/Atomic.h"
#include <vector>

#if defined ION_PLATFORM_WINDOWS
#include "core/platform/windows/thread/LocalStorageWindows.h"
#elif defined ION_PLATFORM_LINUX
#include "core/platform/linux/thread/LocalStorageLinux.h"
#elif defined ION_PLATFORM_MACOSX
#include "core/platform/macosx/thread/LocalStorageMacOSX.h"
#elif defined ION_PLATFORM_DREAMCAST
#include "core/platform/dreamcast/thread/LocalStorageDreamcast.h"
#elif defined ION_PLATFORM_SWITCH
#include "core/platform/nx/thread/LocalStorageSwitch.h"
#elif defined ION_PLATFORM_ANDROID
#include "core/platform/android/thread/LocalStorageAndroid.h"
#endif

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

			LocalStorageImpl m_impl;

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
		}

		template <typename T, int MAX_SIZE> LocalStorage<T, MAX_SIZE>::~LocalStorage()
		{

		}

		template <typename T, int MAX_SIZE> bool LocalStorage<T, MAX_SIZE>::IsAllocated() const
		{
			return m_impl.IsAllocated();
		}

		template <typename T, int MAX_SIZE> void LocalStorage<T, MAX_SIZE>::Store(void* ptr)
		{
			m_impl.Store(ptr);
		}

		template <typename T, int MAX_SIZE> void* LocalStorage<T, MAX_SIZE>::Retrieve()
		{
			return m_impl.Retrieve();
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
