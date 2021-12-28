///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		LocalStorage.cpp
// Date:		10th January 2014
// Authors:		Matt Phillips
// Description:	Threading and synchronisation
///////////////////////////////////////////////////

#include "LocalStorageAndroid.h"
#include "core/debug/Debug.h"

namespace ion
{
	namespace thread
	{
		LocalStorageImpl::LocalStorageImpl()
		{
			pthread_key_create(&m_TLSContext, NULL);
		}

		LocalStorageImpl::~LocalStorageImpl()
		{
			pthread_key_delete(m_TLSContext);
		}

		bool LocalStorageImpl::IsAllocated() const
		{
			return pthread_getspecific(m_TLSContext) != nullptr;
		}

		void LocalStorageImpl::Store(void* ptr)
		{
			pthread_setspecific(m_TLSContext, ptr);
		}

		void* LocalStorageImpl::Retrieve()
		{
			return pthread_getspecific(m_TLSContext);
		}
	}
}