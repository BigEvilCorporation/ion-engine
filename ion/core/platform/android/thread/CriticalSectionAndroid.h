///////////////////////////////////////////////////
// (c) 2020 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		CriticalSection.h
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
		class CriticalSectionImpl
		{
		public:
			CriticalSectionImpl();
			~CriticalSectionImpl();

			bool TryBegin();
			void Begin();
			void End();

		private:
			pthread_mutex_t m_criticalSectionHndl;
		};
	}
}
