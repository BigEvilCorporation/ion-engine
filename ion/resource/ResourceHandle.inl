
////////////////////////////////////////////////////////////////
// Template definitions
////////////////////////////////////////////////////////////////
#include "resource/Resource.h"

namespace ion
{
	namespace io
	{
		template <class T> ResourceHandle<T>::ResourceHandle()
		{
			m_resource = NULL;
		}

		template <class T> ResourceHandle<T>::ResourceHandle(ResourceT<T>* resource)
		{
			m_resource = resource;
			m_resource->Reference();
		}

		template <class T> ResourceHandle<T>::ResourceHandle(const ResourceHandle<T>& rhs)
		{
			m_resource = rhs.m_resource;

			if (m_resource)
			{
				m_resource->Reference();
			}
		}

		template <class T> ResourceHandle<T>::ResourceHandle(T* object)
		{
			m_resource = new ResourceT<T>(object);
		}

		template <class T> ResourceHandle<T>::~ResourceHandle()
		{
			Clear();
		}

		template <class T> bool ResourceHandle<T>::IsValid() const
		{
			return m_resource && m_resource->IsLoaded();
		}

		template <class T> void ResourceHandle<T>::Clear()
		{
			if (m_resource)
			{
				m_resource->Release();
				m_resource = NULL;
			}
		}

		template <class T> ResourceHandle<T>& ResourceHandle<T>::operator = (const ResourceHandle& rhs)
		{
			if (m_resource != rhs.m_resource)
			{
				Clear();
				m_resource = rhs.m_resource;

				if (m_resource)
				{
					m_resource->Reference();
				}
			}

			return *this;
		}

		template <class T> ResourceHandle<T>::operator bool() const
		{
			return IsValid();
		}

		template <class T> T* ResourceHandle<T>::Get() const
		{
			debug::Assert(IsValid(), "Resource not loaded");
			return m_resource->Get();
		}

		template <class T> ResourceHandle<T>::operator T* () const
		{
			return Get();
		}

		template <class T> ResourceHandle<T>::operator T& () const
		{
			return *Get();
		}

		template <class T> T& ResourceHandle<T>::operator * () const
		{
			return *Get();
		}

		template <class T> T* ResourceHandle<T>::operator -> () const
		{
			return Get();
		}

		template <class T> const std::string& ResourceHandle<T>::GetName() const
		{
			debug::Assert(IsValid(), "Resource not loaded");
			return m_resource->GetFilename();
		}
	}
}