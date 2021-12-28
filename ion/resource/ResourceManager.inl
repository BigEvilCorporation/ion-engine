
////////////////////////////////////////////////////////////////
// Template definitions
////////////////////////////////////////////////////////////////
#include "resource/Resource.h"
#include "resource/ResourceHandle.h"

namespace ion
{
	namespace io
	{
		template <class T> void ResourceManager::SetResourceDirectory(const std::string& directory, const std::string& extension)
		{
			std::string typeName = typeid(T).name();
			std::map<std::string, DirectoryEntry>::iterator it = m_directoryMap.find(typeName);

			DirectoryEntry entry;
			entry.directory = directory;
			entry.extension = extension;

			if (it == m_directoryMap.end())
			{
				std::pair<std::map<std::string, DirectoryEntry>::iterator, bool> result = m_directoryMap.insert(std::pair<std::string, DirectoryEntry>(typeName, entry));
				it = result.first;
			}

			it->second = entry;
		}

		template <class T> std::string ResourceManager::GetResourceDirectory() const
		{
			std::string directory = "";

			std::string typeName = typeid(T).name();
			std::map<std::string, DirectoryEntry>::const_iterator it = m_directoryMap.find(typeName);

			if (it != m_directoryMap.end())
			{
				directory = it->second.directory;
			}

			return directory;
		}

		template <class T> std::string ResourceManager::GetResourceExtension() const
		{
			std::string extension = "";

			std::string typeName = typeid(T).name();
			std::map<std::string, DirectoryEntry>::const_iterator it = m_directoryMap.find(typeName);

			if (it != m_directoryMap.end())
			{
				extension = it->second.extension;
			}

			return extension;
		}

		template <class T> ResourceHandle<T> ResourceManager::GetResource(const std::string& filename)
		{
			m_resourceMapLock.Begin();

			std::map<std::string, ResourceEntry*>::iterator it = m_resourceMap.find(filename);

			if (it == m_resourceMap.end())
			{
				//Create new resource
				ResourceT<T>* resource = new ResourceT<T>(*this, filename);

				//Create new bookkeeping entry
				ResourceEntryT<T>* resourceEntry = new ResourceEntryT<T>(resource);

				//Add to map
				std::pair<std::map<std::string, ResourceEntry*>::iterator, bool> result = m_resourceMap.insert(std::pair<std::string, ResourceEntry*>(filename, resourceEntry));
				it = result.first;
			}

			m_resourceMapLock.End();

			//Create handle
			return ResourceHandle<T>((ResourceT<T>*)it->second->m_resource);
		}

		template <class T> ResourceHandle<T> ResourceManager::GetResource(const std::string& filename, std::function<void(T&)> const& onLoaded)
		{
			m_resourceMapLock.Begin();

			std::map<std::string, ResourceEntry*>::iterator it = m_resourceMap.find(filename);

			ResourceEntryT<T>* resourceEntry = nullptr;

			if (it == m_resourceMap.end())
			{
				//Create new resource
				ResourceT<T>* resource = new ResourceT<T>(*this, filename);

				//Create new bookkeeping entry
				resourceEntry = new ResourceEntryT<T>(resource);

				//Add to map
				std::pair<std::map<std::string, ResourceEntry*>::iterator, bool> result = m_resourceMap.insert(std::pair<std::string, ResourceEntry*>(filename, resourceEntry));
				it = result.first;

				//Add callback
				if(onLoaded)
					resourceEntry->Subscribe_OnLoaded(onLoaded);
			}
			else
			{
				//Resource exists
				resourceEntry = (ResourceEntryT<T>*)it->second;

				//Add callback
				if(onLoaded)
					resourceEntry->Subscribe_OnLoaded(onLoaded);

				//Add to callback queue
#if ION_RESOURCE_MGR_MULTITHREADED
				m_workerThread->m_pendingOnLoaded.Push(it->second);
#else
				it->second->Broadcast_OnLoaded();
#endif
			}

			m_resourceMapLock.End();

			//Create handle
			return ResourceHandle<T>((ResourceT<T>*)it->second->m_resource);
		}

		template <class T> ResourceHandle<T> ResourceManager::AddResource(const std::string& filename, T& resourceObject)
		{
			m_resourceMapLock.Begin();

			std::map<std::string, ResourceEntry*>::iterator it = m_resourceMap.find(filename);

			if (it == m_resourceMap.end())
			{
				//Create new bookkeeping entry
				ResourceEntry* resourceEntry = new ResourceEntryT<T>(&resourceObject);

				//Add to map
				std::pair<std::map<std::string, ResourceEntry*>::iterator, bool> result = m_resourceMap.insert(std::pair<std::string, ResourceEntry*>(filename, resourceEntry));
				it = result.first;
			}
			else
			{
				ion::debug::Error("ResourceManager::AddResource() - Resource already exists");
			}

			m_resourceMapLock.End();

			return ResourceHandle<T>((ResourceT<T>*)it->second->m_resource);
		}
	}
}