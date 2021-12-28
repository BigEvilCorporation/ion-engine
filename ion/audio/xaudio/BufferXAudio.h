#pragma once

#include <core/Types.h>

#include "audio/Buffer.h"

namespace ion
{
	namespace audio
	{
		class BufferXAudio : public Buffer
		{
		public:
			BufferXAudio(u32 size);
			BufferXAudio(Buffer& rhs);
			virtual ~BufferXAudio();

			virtual void Add(const char* data, u32 size);
			virtual void Put(const char* data, u32 size, u32 Position);

		private:
		};
	}
}