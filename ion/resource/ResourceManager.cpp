///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		ResourceManager.cpp
// Date:		22nd December 2013
// Authors:		Matt Phillips
// Description:	Resource management
///////////////////////////////////////////////////

#include "resource/ResourceManager.h"
#include "core/thread/Atomic.h"
#include "core/thread/Semaphore.h"

namespace ion
{
	namespace io
	{
		ResourceManager::ResourceManager()
		{
			m_workerThread = new WorkerThread();

#if ION_RESOURCE_MGR_MULTITHREADED
			m_workerThread->Run();
#endif
		}

		ResourceManager::~ResourceManager()
		{
			delete m_workerThread;
		}

		void ResourceManager::RemoveResource(const std::string& filename)
		{
			std::map<std::string, ResourceEntry*>::iterator it = m_resourceMap.find(filename);

			if(it != m_resourceMap.end())
			{
				ion::debug::Assert(it->second->m_resource->GetResourceCount() == 0, "ResourceManager::RemoveResource() - Resource is still referenced");
				m_resourceMap.erase(it);
			}
		}

		u32 ResourceManager::GetNumResourcesWaiting() const
		{
			return m_workerThread->GetNumJobsInQueue();
		}

		void ResourceManager::WaitForResources()
		{
#if ION_RESOURCE_MGR_MULTITHREADED
			m_workerThread->WaitForAllJobs();
#endif
		}

		void ResourceManager::RequestLoad(Resource& resource)
		{
			m_resourceMapLock.Begin();
			std::map<std::string, ResourceEntry*>::iterator it = m_resourceMap.find(resource.GetFilename());
			ion::debug::Assert(it != m_resourceMap.end(), "ResourceManager::RequestLoad() - resource does not exist");
			m_resourceMapLock.End();

#if ION_RESOURCE_MGR_MULTITHREADED
			if(thread::GetCurrentThreadId() == m_workerThread->GetId())
#endif
			{
				//Already on worker thread, do job immediately
				it->second->m_resource->Load();

				//Add to callback queue
#if ION_RESOURCE_MGR_MULTITHREADED
				m_workerThread->m_pendingOnLoaded.Push(it->second);
#else
				it->second->Broadcast_OnLoaded();
#endif
			}
#if ION_RESOURCE_MGR_MULTITHREADED
			else
			{
				//Push to job to worker thread
				WorkerThread::Job job(WorkerThread::Job::JobType::Load, *it->second);
				m_workerThread->PushJob(job);
			}
#endif
		}

		void ResourceManager::RequestUnload(Resource& resource)
		{
			m_resourceMapLock.Begin();
			std::map<std::string, ResourceEntry*>::iterator it = m_resourceMap.find(resource.GetFilename());
			ion::debug::Assert(it != m_resourceMap.end(), "ResourceManager::RequestLoad() - resource does not exist");
			m_resourceMapLock.End();

#if ION_RESOURCE_MGR_MULTITHREADED
			if(thread::GetCurrentThreadId() == m_workerThread->GetId())
#endif
			{
				//Already on worker thread, do job immediately
				it->second->m_resource->Unload();
			}
#if ION_RESOURCE_MGR_MULTITHREADED
			else
			{
				//Push to job to worker thread
				WorkerThread::Job job(WorkerThread::Job::JobType::Unload, *it->second);
				m_workerThread->PushJob(job);
			}
#endif
		}

		void ResourceManager::Update()
		{
			while (!m_workerThread->m_pendingOnLoaded.IsEmpty())
			{
				ResourceEntry* resourceEntry = m_workerThread->m_pendingOnLoaded.Pop();
				resourceEntry->Broadcast_OnLoaded();
			}
		}

		ResourceManager::WorkerThread::WorkerThread()
			: thread::Thread("ResourceManagerWorker")
			, m_jobQueueSemaphore(s_jobQueueSize)
			, m_waitSemaphore(s_jobQueueSize)
		{
			m_numJobsInQueue = 0;
			m_threadRunning = true;
		}

		ResourceManager::WorkerThread::~WorkerThread()
		{
			Terminate();
		}

		void ResourceManager::WorkerThread::PushJob(Job& job)
		{
			m_jobQueue.Push(job);
			thread::atomic::Increment(m_numJobsInQueue);
			m_jobQueueSemaphore.Signal();
		}

		u32 ResourceManager::WorkerThread::GetNumJobsInQueue() const
		{
			return m_numJobsInQueue;
		}

		void ResourceManager::WorkerThread::WaitForAllJobs()
		{
			Job waitJob;
			waitJob.m_jobType = Job::JobType::Wait;
			PushJob(waitJob);
			m_waitSemaphore.Wait();
		}

		void ResourceManager::WorkerThread::Terminate()
		{
			Job shutdownJob;
			shutdownJob.m_jobType = Job::JobType::Shutdown;
			PushJob(shutdownJob);
			Join();
		}

		void ResourceManager::WorkerThread::Entry()
		{
			while(m_threadRunning)
			{
				//Wait for job
				m_jobQueueSemaphore.Wait();

				//Pop job from queue
				Job job = m_jobQueue.Pop();

				switch (job.m_jobType)
				{
					case Job::JobType::Load:
					{
						//Load resource
						job.m_resourceEntry->m_resource->Load();

						//Add to callback queue
						m_pendingOnLoaded.Push(job.m_resourceEntry);
						break;
					}
					case Job::JobType::Unload:
					{
						//Unload resource
						job.m_resourceEntry->m_resource->Unload();
						break;
					}
					case Job::JobType::Wait:
					{
						m_waitSemaphore.Signal();
						break;
					}
					case Job::JobType::Shutdown:
					{
						m_threadRunning = false;
						break;
					}
				}

				thread::atomic::Decrement(m_numJobsInQueue);
			}
		}
	}
}