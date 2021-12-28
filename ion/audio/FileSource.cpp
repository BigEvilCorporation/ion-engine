#include "FileSource.h"

#include <ion/core/thread/Atomic.h>
#include <ion/core/thread/Sleep.h>
#include <ion/audio/Buffer.h>
#include <ion/maths/Maths.h>
#include <ion/core/debug/Debug.h>

namespace ion
{
	namespace audio
	{
		StreamingThread* FileSource::s_streamingThread = nullptr;
		int FileSource::s_threadRefCount = 0;

		FileSource::FileSource(FeedType feedType, FileReader& reader, bool loop)
			: Source(feedType),
			m_fileReader(reader)
		{
			m_streamDesc = &m_fileReader.GetStreamDesc();
			m_buffer = nullptr;
			m_loop = loop;
			m_debugName = m_fileReader.GetFilename();

			for (int i = 0; i < s_numStreamBuffers; i++)
			{
				m_streamBuffers[i] = nullptr;
			}

			if (feedType == FeedType::Streaming)
			{
				//Start streaming thread
				if (s_threadRefCount == 0)
				{
					s_streamingThread = new StreamingThread();
					s_streamingThread->Run();
				}

				s_threadRefCount++;
			}
		}

		FileSource::~FileSource()
		{

		}

		bool FileSource::Load()
		{
			if (m_feedType == FeedType::SingleBuffer)
			{
				//Open file
				if (m_fileReader.Open())
				{
					//Set stream desc
					m_streamDesc = &m_fileReader.GetStreamDesc();

					//Alloc buffer
					m_buffer = Buffer::Create(m_streamDesc->GetEncodedSizeBytes());

					//Lock buffer while stream is open
					m_buffer->WriteLock();

					//Read data
					m_fileReader.Read(m_buffer->Get(0), m_streamDesc->GetEncodedSizeBytes());

					//Close file
					m_fileReader.Close();

					return true;
				}
			}

			return false;
		}

		bool FileSource::OpenStream(OnStreamOpened const& onOpened)
		{
			if (m_feedType == FeedType::Streaming)
			{
				m_onOpenedCallback = onOpened;

				//Send open request
				s_streamingThread->RequestOpen(*this);

				return true;
			}

			return false;
		}

		void FileSource::CloseStream(OnStreamClosed const& onClosed)
		{
			if (m_feedType == FeedType::SingleBuffer)
			{
				if (m_buffer)
				{
					m_buffer->WriteUnlock();
					delete m_buffer;
					m_buffer = nullptr;
				}
			}
			else if (m_feedType == FeedType::Streaming)
			{
				m_onClosedCallback = onClosed;

				//Send close request
				s_streamingThread->RequestClose(*this);
			}
		}

		void FileSource::StreamThreadOpen()
		{
			bool success = false;

			if (m_fileReader.Open())
			{
				//Alloc buffers
				for (int i = 0; i < s_numStreamBuffers; i++)
				{
					m_streamBuffers[i] = Buffer::Create(s_streamBufferSize);
					m_streamBuffers[i]->WriteLock();
					m_streamBuffers[i]->Reserve(s_streamBufferSize);
					m_streamBuffers[i]->WriteUnlock();
				}

				//Read first buffer
				StreamThreadReadNextBuffer();

				success = true;
			}

			//Issue callback
			if (m_onOpenedCallback)
			{
				m_onOpenedCallback(*this, success);
			}
		}

		void FileSource::StreamThreadClose()
		{
			for (int i = 0; i < s_numStreamBuffers; i++)
			{
				if (m_streamBuffers[i])
				{
					delete m_streamBuffers[i];
					m_streamBuffers[i] = nullptr;
				}
			}

			m_fileReader.Close();

			//Issue callback
			if (m_onClosedCallback)
			{
				m_onClosedCallback(*this, true);
			}
		}

		void FileSource::StreamThreadReadNextBuffer()
		{
			//Read buffer
			Buffer* buffer = m_streamBuffers[m_bufferIdxProducer % s_numStreamBuffers];
			buffer->ReadLock();

			u32 bytesRemaining = s_streamBufferSize;
			u32 bufferPosition = 0;

			while (bytesRemaining)
			{
				int bytesToRead = ion::maths::Min(bytesRemaining, GetStreamDesc()->GetDecodedSizeBytes() - m_fileReader.GetPosition());
				int bytesRead = m_fileReader.Read(buffer->Get(bufferPosition), bytesToRead);

				if (bytesRead < bytesToRead || bytesRead == 0)
				{
					if (m_loop)
					{
						//Looping and at end, seek to start and continue
						m_fileReader.SeekRaw(0);
					}
					else
					{
						//TODO: End of stream
						bytesRemaining = 0;
					}
				}

				bytesRemaining -= bytesRead;
				bufferPosition += bytesRead;
			}

			buffer->ReadUnlock();

			//Next buffer
			ion::thread::atomic::Increment(m_bufferIdxProducer);
		}

		void FileSource::RequestBuffer(SourceCallback& callback)
		{
			if(m_feedType == FeedType::SingleBuffer)
			{
				callback.SubmitBuffer(*m_buffer);
			}
			else
			{
				//Submit next buffer
				callback.SubmitBuffer(*m_streamBuffers[m_bufferIdxConsumer % s_numStreamBuffers]);

				//Next buffer
				ion::thread::atomic::Increment(m_bufferIdxConsumer);

				//Request next
				s_streamingThread->RequestBuffer(*this);
			}
		}

		StreamingThread::StreamingThread()
			: ion::thread::Thread("AudioStreaming")
			, m_jobSemaphore(s_maxJobs)
		{

		}

		StreamingThread::~StreamingThread()
		{
			Job job;
			job.jobType = JobType::Shutdown;
			PushJob(job);
		}

		void StreamingThread::RequestOpen(FileSource& fileSource)
		{
			Job job;
			job.jobType = JobType::Open;
			job.fileSource = &fileSource;
			PushJob(job);
		}

		void StreamingThread::RequestClose(FileSource& fileSource)
		{
			Job job;
			job.jobType = JobType::Close;
			job.fileSource = &fileSource;
			PushJob(job);
		}

		void StreamingThread::RequestBuffer(FileSource& fileSource)
		{
			Job job;
			job.jobType = JobType::Read;
			job.fileSource = &fileSource;
			PushJob(job);
		}

		void StreamingThread::Entry()
		{
			bool running = true;

			while (running)
			{
				Job job = PopJob();

				switch (job.jobType)
				{
				case JobType::Open:
					job.fileSource->StreamThreadOpen();
					break;
				case JobType::Close:
					job.fileSource->StreamThreadClose();
					break;
				case JobType::Read:
					job.fileSource->StreamThreadReadNextBuffer();
					break;
				case JobType::Shutdown:
					running = false;
					break;
				}
			}
		}

		void StreamingThread::PushJob(Job& job)
		{
			m_jobCriticalSection.Begin();
			m_jobs.Push(job);
			m_jobCriticalSection.End();
			m_jobSemaphore.Signal();
		}

		StreamingThread::Job StreamingThread::PopJob()
		{
			m_jobSemaphore.Wait();
			m_jobCriticalSection.Begin();
			Job job = m_jobs.Pop();
			m_jobCriticalSection.End();
			return job;
		}
	}
}