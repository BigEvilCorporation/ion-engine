///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Compression.h
// Date:		12th January 2018
// Authors:		Matt Phillips
// Description:	Compressor/decompressor interfaces
///////////////////////////////////////////////////

#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "core/Types.h"
#include <vector>

namespace ion
{
	namespace io
	{
		template <typename ELEMENT_T> class Compressor
		{
		public:
			virtual int Compress(const std::vector<ELEMENT_T>& uncompressedData, std::vector<u8>& compressedData) = 0;
		};

		template <typename ELEMENT_T> class Decompressor
		{
		public:
			virtual int Decompress(const std::vector<u8>& compressedData, std::vector<ELEMENT_T>& decompressedData) = 0;
		};
	}
}

#endif // Header guard