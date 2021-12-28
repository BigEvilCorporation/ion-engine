#pragma once

#include <core/Types.h>

namespace ion
{
	namespace audio
	{
		class Buffer
		{
		public:
			static Buffer* Create(u32 size);
			static Buffer* Create(Buffer& rhs);

			virtual ~Buffer();

			Buffer& operator = (Buffer& rhs);

			void ReadLock();
			void ReadUnlock();

			void WriteLock();
			void WriteUnlock();

			virtual void Add(const char* data, u32 size) = 0;
			virtual void Put(const char* data, u32 size, u32 Position) = 0;

			void Reserve(u32 size);
			char* Get(u32 Position) const;
			void Clear();
			void Reset();

			u32 GetReservedSize() const;
			virtual u32 GetDataSize() const;

		protected:
			Buffer(u32 size);

			char* m_data;
			u32 m_reservedSize;
			u32 m_dataSize;
			u32 m_lockCountWrite;
			u32 m_lockCountRead;
		};
	}
}