///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Stream.h
// Date:		22nd November 2012
// Authors:		Matt Phillips
// Description:	Read/write stream for serialisation
///////////////////////////////////////////////////

#ifndef STREAM_H
#define STREAM_H

#include "core/Types.h"

#include <vector>

namespace ion
{
	namespace io
	{
		enum SeekMode { eSeekModeStart, eSeekModeCurrent };

		class Stream
		{
		public:
			virtual s64 GetSize() const = 0;
			virtual s64 GetPosition() const = 0;
			virtual s64 Read(void* data, s64 size) = 0;
			virtual s64 Write(const void* data, s64 size) = 0;
			virtual s64 Seek(s64 position, SeekMode origin = eSeekModeCurrent) = 0;
		};

		class MemoryStream : public Stream
		{
		public:
			MemoryStream();
			virtual s64 GetSize() const;
			virtual s64 GetPosition() const;
			virtual s64 Read(void* data, s64 size);
			virtual s64 Write(const void* data, s64 size);
			virtual s64 Seek(s64 position, SeekMode origin = eSeekModeCurrent);

			MemoryStream& operator = (const MemoryStream& rhs);

		private:
			std::vector<u8> m_bytes;
			s64 m_position;
		};
	}
}

#endif
