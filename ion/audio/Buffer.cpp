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
			m_lockCount = 0;
			m_dataSize = 0;
			m_reservedSize = size;
			m_data = new char[size];
			memory::MemSet(m_data, 0, size);
		}

		Buffer::~Buffer()
		{
			debug::Assert(m_lockCount == 0, "Buffer::~Buffer() - Buffer still locked");
			delete m_data;
		}

		void Buffer::Lock()
		{
			debug::Assert(m_lockCount == 0, "Buffer:Lock() - Buffer already locked");
			thread::atomic::Increment(m_lockCount);
		}

		void Buffer::Unlock()
		{
			debug::Assert(m_lockCount > 0, "Buffer::Unlock() - Buffer not locked");
			thread::atomic::Decrement(m_lockCount);
		}

		void Buffer::Add(const char* data, u32 size)
		{
			debug::Assert(m_lockCount > 0, "Buffer::Add() - Buffer not locked");
			debug::Assert((size + m_dataSize) <= m_reservedSize, "Buffer::Add() - Not enough space");
			memory::MemCopy(m_data + m_dataSize, (void*)data, size);
			m_dataSize += size;
		}

		void Buffer::Put(const char* data, u32 size, u32 position)
		{
			debug::Assert(m_lockCount > 0, "Buffer::Put() - Buffer not locked");
			debug::Assert((size + position) <= m_reservedSize, "Buffer::Put() - Not enough space");
			memory::MemCopy(m_data + position, (void*)data, size);
		}

		char* Buffer::Get(u32 position) const
		{
			debug::Assert(m_lockCount > 0, "Buffer::Get() - Buffer not locked");
			debug::Assert(position < m_reservedSize, "Buffer::Get() - Overflow");
			return m_data + position;
		}

		void Buffer::Clear()
		{
			debug::Assert(m_lockCount > 0, "Buffer::Clear() - Buffer not locked");
			memory::MemSet(m_data, 0, m_dataSize);
			m_dataSize = 0;
		}

		void Buffer::Reset()
		{
			debug::Assert(m_lockCount > 0, "Buffer::Reset() - Buffer not locked");
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