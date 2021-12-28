///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		IndexBuffer.cpp
// Date:		4rd December 2013
// Authors:		Matt Phillips
// Description:	Index buffer class
///////////////////////////////////////////////////

#include "IndexBuffer.h"

namespace ion
{
	namespace render
	{
		IndexBuffer::IndexBuffer()
		{

		}

		void IndexBuffer::Add(TIndex index)
		{
			m_indices.push_back(index);
		}

		void IndexBuffer::Add(TIndex face0, TIndex face1, TIndex face2)
		{
			m_indices.push_back(face0);
			m_indices.push_back(face1);
			m_indices.push_back(face2);
		}

		TIndex IndexBuffer::Get(int index) const
		{
			return m_indices[index];
		}

		int IndexBuffer::GetSize() const
		{
			return (int)m_indices.size();
		}

		const TIndex* IndexBuffer::GetAddress() const
		{
			return &m_indices[0];
		}

		void IndexBuffer::Clear()
		{
			m_indices.clear();
		}

		void IndexBuffer::Reserve(int size)
		{
			m_indices.reserve(size);
		}
	}
}
