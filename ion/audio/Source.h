#pragma once

#include <audio/Callback.h>
#include <core/containers/Queue.h>

#include <string>
#include <functional>

namespace ion
{
	namespace audio
	{
		class Buffer;
		class StreamDesc;

		class Source
		{
			static const int sMaxBufferQueueSize = 8;

		public:
			enum class FeedType
			{
				SingleBuffer,
				Streaming
			};

			typedef std::function<void(Source&,bool)> OnStreamOpened;
			typedef std::function<void(Source&, bool)> OnStreamClosed;

			Source(FeedType feedType);
			virtual ~Source();

			virtual bool OpenStream(OnStreamOpened const& onOpened) = 0;
			virtual void CloseStream(OnStreamClosed const& onClosed) = 0;

			void Lock();
			void Unlock();

			const StreamDesc* GetStreamDesc() const;
			FeedType GetFeedType() const;

			virtual void RequestBuffer(SourceCallback& callback) = 0;

		protected:
			const StreamDesc* m_streamDesc;
			FeedType m_feedType;
			u32 m_lockCount;
			std::string m_debugName;
		};
	}
}