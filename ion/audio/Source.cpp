#include <core/debug/Debug.h>

#include "Source.h"

namespace ion
{
	namespace audio
	{
		Source::Source(FeedType feedType)
		{
			m_streamDesc = nullptr;
			m_feedType = feedType;
			m_lockCount = 0;
		}

		Source::~Source()
		{
		}

		void Source::Lock()
		{
			thread::atomic::Increment(m_lockCount);
		}

		void Source::Unlock()
		{
			debug::Assert(m_lockCount > 0, "Source::Unlock() - Source not locked");
			thread::atomic::Decrement(m_lockCount);
		}

		const StreamDesc* Source::GetStreamDesc() const
		{
			return m_streamDesc;
		}

		Source::FeedType Source::GetFeedType() const
		{
			return m_feedType;
		}
	}
}