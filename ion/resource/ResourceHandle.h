///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Resource.h
// Date:		22nd December 2013
// Authors:		Matt Phillips
// Description:	Resource management
///////////////////////////////////////////////////

#pragma once

#include "core/Types.h"
#include "core/debug/Debug.h"

#include <string>

namespace ion
{
	namespace io
	{
		template <class T> class ResourceT;

		template <class T> class ResourceHandle
		{
		public:
			ResourceHandle();
			ResourceHandle(const ResourceHandle<T>& rhs);
			ResourceHandle(T* object);
			~ResourceHandle();

			bool IsValid() const;
			void Clear();

			ResourceHandle<T>& operator = (const ResourceHandle& rhs);

			operator bool () const;
			T& operator * () const;
			T* operator -> () const;
			operator T* () const;
			operator T& () const;

			T* Get() const;

			const std::string& GetName() const;

		protected:
			ResourceHandle(ResourceT<T>* resource);
			ResourceT<T>* m_resource;

			friend class ResourceManager;
		};
	}
}
