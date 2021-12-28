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

namespace ion
{
	namespace thread
	{
		class LocalStorageImpl
		{
		public:
			LocalStorageImpl();
			~LocalStorageImpl();

			bool IsAllocated() const;

			void Store(void* ptr);
			void* Retrieve();

		private:
			DWORD m_TLSContext;
		};
	}
}