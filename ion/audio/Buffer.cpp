#include <core/memory/Memory.h>
#include <core/thread/Atomic.h>
#include <core/debug/Debug.h>

#include "Buffer.h"

namespace ion
{
	namespace audio
	{
		Buffer::Buffer(u32 size)
		{
			m_lockCountRead = 0;
			m_lockCountWrite = 0;
			m_dataSize = 0;
			m_reservedSize = size;
		}

		Buffer::~Buffer()
		{

		}

		Buffer& Buffer::operator = (Buffer& rhs)
		{
			//Copy data from other buffer
			rhs.ReadLock();
			WriteLock();
			Reset();
			Add(rhs.Get(0), rhs.GetDataSize());
			WriteUnlock();
			rhs.ReadUnlock();

			return *this;
		}

		void Buffer::ReadLock()
		{
			//Can read lock multiple times, but not if already locked for writing
			debug::Assert(m_lockCountWrite == 0, "Buffer:WriteLock() - Buffer already locked for writing");
			thread::atomic::Increment(m_lockCountRead);
		}

		void Buffer::ReadUnlock()
		{
			debug::Assert(m_lockCountRead > 0, "Buffer::Unlock() - Buffer not locked for reading");
			thread::atomic::Decrement(m_lockCountRead);
		}

		void Buffer::WriteLock()
		{
			//Can only lock for writing once, and not if already locked for reading
			debug::Assert(m_lockCountWrite == 0, "Buffer:WriteLock() - Buffer already locked for writing");
			debug::Assert(m_lockCountRead == 0, "Buffer:WriteLock() - Buffer already locked for reading");
			thread::atomic::Increment(m_lockCountWrite);
		}

		void Buffer::WriteUnlock()
		{
			debug::Assert(m_lockCountWrite > 0, "Buffer::Unlock() - Buffer not locked for writing");
			thread::atomic::Decrement(m_lockCountWrite);
		}

		void Buffer::Reserve(u32 size)
		{
			debug::Assert(m_lockCountWrite > 0, "Buffer::Reserve() - Buffer not locked for writing");
			debug::Assert((size + m_dataSize) <= m_reservedSize, "Buffer::Reserve() - Not enough space");
			char* start = m_data + m_dataSize;
			m_dataSize += size;
			memory::MemSet(start, 0, size);
		}

		char* Buffer::Get(u32 Position) const
		{
			debug::Assert(m_lockCountRead > 0, "Buffer::Get() - Buffer not locked for reading");
			debug::Assert(Position < m_reservedSize, "Buffer::Get() - Overflow");
			return m_data + Position;
		}

		void Buffer::Clear()
		{
			debug::Assert(m_lockCountWrite > 0, "Buffer::Clear() - Buffer not locked for writing");
			memory::MemSet(m_data, 0, m_dataSize);
			m_dataSize = 0;
		}

		void Buffer::Reset()
		{
			debug::Assert(m_lockCountWrite > 0, "Buffer::Reset() - Buffer not locked for writing");
			m_dataSize = 0;
		}

		u32 Buffer::GetReservedSize() const
		{
			return m_reservedSize;
		}

		u32 Buffer::GetDataSize() const
		{
			return m_dataSize;
		}
	}
}