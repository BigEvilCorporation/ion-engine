#include "BufferXAudio.h"

#include <core/memory/Memory.h>
#include <core/debug/Debug.h>
#include <maths/Maths.h>

namespace ion
{
	namespace audio
	{
		Buffer* Buffer::Create(u32 size)
		{
			return new BufferXAudio(size);
		}

		Buffer* Buffer::Create(Buffer& rhs)
		{
			return new BufferXAudio(rhs);
		}

		BufferXAudio::BufferXAudio(u32 size)
			: Buffer(size)
		{
			m_data = new char[size];
			memory::MemSet(m_data, 0, size);
		}

		BufferXAudio::BufferXAudio(Buffer& rhs)
			: BufferXAudio(rhs.GetReservedSize())
		{
			//Copy data from other buffer
			rhs.ReadLock();
			WriteLock();
			Reset();
			Add(rhs.Get(0), rhs.GetDataSize());
			WriteUnlock();
			rhs.ReadUnlock();
		}

		BufferXAudio::~BufferXAudio()
		{
			debug::Assert(m_lockCountRead == 0, "BufferXAudio::~BufferXAudio() - Buffer still locked for reading");
			debug::Assert(m_lockCountWrite == 0, "BufferXAudio::~BufferXAudio() - Buffer still locked for writing");
			delete m_data;
		}

		void BufferXAudio::Add(const char* data, u32 size)
		{
			debug::Assert(m_lockCountWrite > 0, "Buffer::Add() - Buffer not locked for writing");
			debug::Assert((size + m_dataSize) <= m_reservedSize, "Buffer::Add() - Not enough space");

			memory::MemCopy(m_data + m_dataSize, (void*)data, size);
			m_dataSize += size;
		}

		void BufferXAudio::Put(const char* data, u32 size, u32 Position)
		{
			debug::Assert(m_lockCountWrite > 0, "Buffer::Put() - Buffer not locked for writing");
			debug::Assert((size + Position) <= m_reservedSize, "Buffer::Put() - Not enough space");

			memory::MemCopy(m_data + Position, (void*)data, size);
		}
	}
}