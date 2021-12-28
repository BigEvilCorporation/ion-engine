#pragma once

#include <core/thread/Thread.h>
#include <core/thread/Semaphore.h>
#include <core/thread/CriticalSection.h>
#include <core/containers/Queue.h>
#include <audio/Buffer.h>
#include <audio/FileReader.h>
#include <audio/Source.h>

namespace ion
{
	namespace audio
	{
		class StreamingThread;

		class FileSource : public Source
		{
		public:
			FileSource(FeedType feedType, FileReader& reader, bool loop);
			virtual ~FileSource();

			//Load whole file
			bool Load();

			//Open/close for streaming
			virtual bool OpenStream(OnStreamOpened const& onOpened);
			virtual void CloseStream(OnStreamClosed const& onClosed);

			//Get next buffer
			virtual void RequestBuffer(SourceCallback& callback);

		protected:
			static const int s_streamBufferSize = (1024 * 256);
			static const int s_numStreamBuffers = 3;

			void StreamThreadOpen();
			void StreamThreadClose();
			void StreamThreadReadNextBuffer();

			friend class StreamingThread;

		private:
			FileReader& m_fileReader;
			bool m_loop;
			Buffer* m_buffer;
			Buffer* m_streamBuffers[s_numStreamBuffers];

			u32 m_bufferIdxProducer;
			u32 m_bufferIdxConsumer;

			OnStreamOpened m_onOpenedCallback;
			OnStreamClosed m_onClosedCallback;

			static StreamingThread* s_streamingThread;
			static int s_threadRefCount;
		};

		class StreamingThread : public ion::thread::Thread
		{
		public:
			StreamingThread();
			virtual ~StreamingThread();

			void RequestOpen(FileSource& fileSource);
			void RequestClose(FileSource& fileSource);
			void RequestBuffer(FileSource& fileSource);

		private:
			static const int s_maxJobs = 16;

			enum class JobType
			{
				Open,
				Close,
				Read,
				Shutdown
			};

			struct Job
			{
				JobType jobType;
				FileSource* fileSource;
			};

			virtual void Entry();
			void PushJob(Job& job);
			Job PopJob();

			ion::Queue<Job, s_maxJobs> m_jobs;
			ion::thread::Semaphore m_jobSemaphore;
			ion::thread::CriticalSection m_jobCriticalSection;
		};
	}
}