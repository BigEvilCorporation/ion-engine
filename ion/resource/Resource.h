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
		class ResourceManager;

		class Resource
		{
		public:
			virtual ~Resource();

			void Reference();
			void Release();
			u32 GetResourceCount() const;

			const std::string& GetFilename() const;

			virtual bool Load() { return false; }
			virtual void Unload() {};
			virtual bool IsLoaded() { return false; }

			bool IsManagedResource() const { return m_resourceManager != nullptr; }

		protected:
			Resource();
			Resource(ResourceManager& resourceManager, const std::string& filename);

			ResourceManager* m_resourceManager;
			std::string m_filename;

			u32 m_resourceCount;
			bool m_isLoaded;

			friend class ResourceManager;
		};

		template <class T> class ResourceT : public Resource
		{
		public:
			ResourceT(ResourceManager& resourceManager, const std::string& filename)
				: Resource(resourceManager, filename)
			{
				m_resourceObject = NULL;
			}

			ResourceT(ResourceManager& resourceManager, const std::string& filename, T* resourceObject)
				: Resource(resourceManager, filename)
			{
				m_resourceObject = resourceObject;
				m_isLoaded = true;
			}

			ResourceT(T* object)
			{
				m_resourceObject = object;
				m_isLoaded = true;
			}

			virtual ~ResourceT() {}

			virtual bool Load();
			virtual void Unload();
			virtual bool IsLoaded() { return m_isLoaded; }

			T* Get() const { return m_resourceObject; }

		protected:
			T* m_resourceObject;
		};
	}
}
