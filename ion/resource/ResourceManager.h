///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		ResourceManager.h
// Date:		22nd December 2013
// Authors:		Matt Phillips
// Description:	Resource management
///////////////////////////////////////////////////

#pragma once

#include "core/Types.h"
#include "core/thread/Thread.h"
#include "core/thread/CriticalSection.h"
#include "core/thread/Semaphore.h"
#include "core/containers/Queue.h"
#include "Resource.h"

#include <map>
#include <string>
#include <vector>
#include <functional>

#define ION_RESOURCE_MGR_MULTITHREADED 1

namespace ion
{
	namespace io
	{
		template <class T> class ResourceHandle;

		class ResourceManager
		{
		public:
			ResourceManager();
			~ResourceManager();

			//Set/get resource directory
			template <class T> void SetResourceDirectory(const std::string& directory, const std::string& extension);
			template <class T> std::string GetResourceDirectory() const;
			template <class T> std::string GetResourceExtension() const;

			//Get resource handle
			template <class T> ResourceHandle<T> GetResource(const std::string& filename);
			template <class T> ResourceHandle<T> GetResource(const std::string& filename, std::function<void(T&)> const& onLoaded);

			//Manually add/remove resource
			template <class T> ResourceHandle<T> AddResource(const std::string& filename, T& resourceObject);
			void RemoveResource(const std::string& filename);

			//Get number of resources in thread queue
			u32 GetNumResourcesWaiting() const;

			//Wait for all resources
			void WaitForResources();

			//Issue callbacks for completed loads/unloads
			void Update();

		protected:

			//Bookkeeping data
			struct ResourceEntry
			{
				ResourceEntry(Resource* resource)
					: m_resource(resource) {}

				virtual void Broadcast_OnLoaded() = 0;

				Resource* m_resource;
			};

			//Add load/unload requests to thread queue
			void RequestLoad(Resource& resource);
			void RequestUnload(Resource& resource);

			//Thread worker
			class WorkerThread : public thread::Thread
			{
			private:
				static const int s_jobQueueSize = 1024;

			public:

				struct Job
				{
					enum class JobType { Load, Unload, Wait, Shutdown };

					Job() {}
					Job(JobType jobType, ResourceEntry& resourceEntry)
					{
						m_jobType = jobType;
						m_resourceEntry = &resourceEntry;
					}

					JobType m_jobType;
					ResourceEntry* m_resourceEntry;
				};

				WorkerThread();
				~WorkerThread();

				void PushJob(Job& job);
				u32 GetNumJobsInQueue() const;
				void WaitForAllJobs();

				void Terminate();

				//Callbacks waiting on main thread
				Queue<ResourceEntry*, s_jobQueueSize> m_pendingOnLoaded;

			protected:
				virtual void Entry();

			private:
				Queue<Job, s_jobQueueSize> m_jobQueue;
				thread::Semaphore m_jobQueueSemaphore;
				thread::Semaphore m_waitSemaphore;
				u32 m_numJobsInQueue;
				bool m_threadRunning;
			};

			//Directories
			struct DirectoryEntry
			{
				std::string directory;
				std::string extension;
			};

			template <class T> struct ResourceEntryT : public ResourceEntry
			{
				ResourceEntryT(Resource* resource)
					: ResourceEntry(resource) {}

				void Subscribe_OnLoaded(std::function<void(T&)> const& onLoaded)
				{
					m_onLoaded.push_back(onLoaded);
				}

				virtual void Broadcast_OnLoaded()
				{
					auto copy = std::move(m_onLoaded);

					for(auto callback : copy)
					{
						callback(*(T*)((ResourceT<T>*)m_resource)->Get());
					}
				}

				std::vector<std::function<void(T&)>> m_onLoaded;
			};

			//Directories
			std::map<std::string, DirectoryEntry> m_directoryMap;

			//Resources
			std::map<std::string, ResourceEntry*> m_resourceMap;

			//Worker thread
			WorkerThread* m_workerThread;

			//Resource map lock
			ion::thread::CriticalSection m_resourceMapLock;

			friend class Resource;
		};
	}
}

//Template/inline includes
#include "ResourceManager.inl"
#include "ResourceHandle.inl"
#include "Resource.inl"