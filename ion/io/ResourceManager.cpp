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

#include "io/ResourceManager.h"
#include "core/thread/Atomic.h"

namespace ion
{
	namespace io
	{
		ResourceManager::ResourceManager()
		{
			m_workerThread = new WorkerThread();
			m_workerThread->Run();
		}

		ResourceManager::~ResourceManager()
		{
			delete m_workerThread;
		}

		void ResourceManager::RemoveResource(const std::string& filename)
		{
			std::map<std::string, Resource*>::iterator it = m_resourceMap.find(filename);

			if(it != m_resourceMap.end())
			{
				ion::debug::Assert(it->second->GetResourceCount() == 0, "ResourceManager::RemoveResource() - Resource is still referenced");
				m_resourceMap.erase(it);
			}
		}

		u32 ResourceManager::GetNumResourcesWaiting() const
		{
			return m_workerThread->GetNumJobsInQueue();
		}

		void ResourceManager::RequestLoad(Resource& resource)
		{
			if(thread::GetCurrentThreadId() == m_workerThread->GetId())
			{
				//Already on worker thread, do job immediately
				resource.Load();
			}
			else
			{
				//Push to job to worker thread
				WorkerThread::Job job(WorkerThread::Job::Load, resource);
				m_workerThread->PushJob(job);
			}
		}

		void ResourceManager::RequestUnload(Resource& resource)
		{
			if(thread::GetCurrentThreadId() == m_workerThread->GetId())
			{
				//Already on worker thread, do job immediately
				resource.Unload();
			}
			else
			{
				//Push to job to worker thread
				WorkerThread::Job job(WorkerThread::Job::Unload, resource);
				m_workerThread->PushJob(job);
			}
		}

		ResourceManager::WorkerThread::WorkerThread()
			: thread::Thread("ResourceManagerWorker")
			, m_jobQueueSemaphore(s_jobQueueSize)
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

		void ResourceManager::WorkerThread::Terminate()
		{
			Job shutdownJob;
			shutdownJob.m_jobType = Job::Shutdown;
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
					case Job::Load:
					{
						//Load resource
						job.m_resource->Load();
						break;
					}
					case Job::Unload:
					{
						//Unload resource
						job.m_resource->Unload();
						break;
					}
					case Job::Shutdown:
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