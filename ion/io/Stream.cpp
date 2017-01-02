///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Stream.cpp
// Date:		22nd November 2012
// Authors:		Matt Phillips
// Description:	Read/write stream for serialisation
///////////////////////////////////////////////////

#include "Stream.h"
#include "core/memory/Memory.h"
#include "core/debug/Debug.h"

#include <algorithm>
#include <utility>

namespace ion
{
	namespace io
	{
		MemoryStream::MemoryStream()
		{
			m_position = 0;
		}

		s64 MemoryStream::GetSize() const
		{
			return m_bytes.size();
		}

		s64 MemoryStream::GetPosition() const
		{
			return m_position;
		}

		s64 MemoryStream::Read(void* data, s64 size)
		{
			size = std::min(size, GetSize() - m_position);
			memory::MemCopy(data, &m_bytes[m_position], size);
			m_position += size;
			return size;
		}

		s64 MemoryStream::Write(const void* data, s64 size)
		{
			if(size > 0)
			{
				s64 requiredSize = m_position + size;
				if(requiredSize > GetSize())
					m_bytes.resize(requiredSize);

				memory::MemCopy(&m_bytes[m_position], data, size);
				m_position += size;
			}
			
			return size;
		}

		s64 MemoryStream::Seek(s64 position, SeekMode origin)
		{
			debug::Assert(position < GetSize(), "MemoryStream::Seek() - eOut of range");
			m_position = position;
			return m_position;
		}

		MemoryStream& MemoryStream::operator = (const MemoryStream& rhs)
		{
			m_bytes = rhs.m_bytes;
			m_position = rhs.m_position;
			return *this;
		}
	}
}