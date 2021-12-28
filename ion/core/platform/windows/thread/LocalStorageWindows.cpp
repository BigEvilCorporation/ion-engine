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

#include "LocalStorageWindows.h"
#include "core/debug/Debug.h"

namespace ion
{
	namespace thread
	{
		LocalStorageImpl::LocalStorageImpl()
		{
			m_TLSContext = TlsAlloc();
			debug::Assert(m_TLSContext != TLS_OUT_OF_INDEXES, "Could not create TLS - out of indices");
		}

		LocalStorageImpl::~LocalStorageImpl()
		{
			TlsFree(m_TLSContext);
		}

		bool LocalStorageImpl::IsAllocated() const
		{
			return TlsGetValue(m_TLSContext) != nullptr;
		}

		void LocalStorageImpl::Store(void* ptr)
		{
			TlsSetValue(m_TLSContext, ptr);
		}

		void* LocalStorageImpl::Retrieve()
		{
			return TlsGetValue(m_TLSContext);
		}
	}
}