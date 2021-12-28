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
#if defined ION_PLATFORM_WINDOWS || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_LINUX || defined ION_PLATFORM_SWITCH || defined ION_PLATFORM_ANDROID
		typedef u32 TIndex;
#elif defined ION_PLATFORM_DREAMCAST || defined ION_PLATFORM_RASPBERRYPI
		typedef u16 TIndex;
#endif

		class IndexBuffer
		{
		public:
			IndexBuffer();

			void Add(TIndex index);
			void Add(TIndex face0, TIndex face1, TIndex face2);

			TIndex Get(int index) const;

			int GetSize() const;
			const TIndex* GetAddress() const;

			void Clear();
			void Reserve(int size);

		private:
			std::vector<TIndex> m_indices;
		};
	}
}
