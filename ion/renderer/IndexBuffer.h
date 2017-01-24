///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		IndexBuffer.h
// Date:		4rd December 2013
// Authors:		Matt Phillips
// Description:	Index buffer class
///////////////////////////////////////////////////

#pragma once

#include "core/Types.h"

#include <vector>

namespace ion
{
	namespace render
	{
#if defined ION_PLATFORM_WINDOWS
		typedef u32 TIndex;
#elif defined ION_PLATFORM_MACOSX
        typedef u32 TIndex;
#elif defined ION_PLATFORM_DREAMCAST
		typedef u16 TIndex;
#endif

		class IndexBuffer
		{
		public:
			void Add(TIndex index) { m_indices.push_back(index); }
			void Add(TIndex face0, TIndex face1, TIndex face2)
			{
				m_indices.push_back(face0);
				m_indices.push_back(face1);
				m_indices.push_back(face2);
			}

			TIndex Get(int index) const { return m_indices[index]; }

			int GetSize() const { return m_indices.size(); }
			const TIndex* GetAddress() const { return &m_indices[0]; }

		private:
			std::vector<TIndex> m_indices;
		};
	}
}
