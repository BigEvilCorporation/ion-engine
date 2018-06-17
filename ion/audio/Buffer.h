#pragma once

#include <core/Types.h>

namespace ion
{
	namespace audio
	{
		class Buffer
		{
		public:
			Buffer(u32 size);
			~Buffer();

			void Lock();
			void Unlock();

			void Add(const char* data, u32 size);
			void Put(const char* data, u32 size, u32 position);
			char* Get(u32 position) const;
			void Clear();
			void Reset();

			u32 GetReservedSize() const;
			u32 GetDataSize() const;

		private:
			char* m_data;
			u32 m_reservedSize;
			u32 m_dataSize;
			u32 m_lockCount;
		};
	}
}