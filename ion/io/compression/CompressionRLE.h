///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		CompressionRLE.h
// Date:		12th January 2018
// Authors:		Matt Phillips
// Description:	Run length encoding compressor/decompressor
///////////////////////////////////////////////////

#ifndef COMPRESSIONRLE_H
#define COMPRESSIONRLE_H

#include "Compression.h"
#include <ion/core/debug/Debug.h>

namespace ion
{
	namespace io
	{
		template <typename ELEMENT_T, int SIZE_BITS> class CompressorRLE : public Compressor<ELEMENT_T>
		{
		public:
			virtual int Compress(const std::vector<ELEMENT_T>& uncompressedData, std::vector<u8>& compressedData);
		};

		template <typename ELEMENT_T, int SIZE_BITS> class DecompressorRLE : public Decompressor<ELEMENT_T>
		{
		public:
			virtual int Decompress(const std::vector<u8>& compressedData, std::vector<ELEMENT_T>& decompressedData);
		};
	}
}

namespace ion
{
	namespace io
	{
		int WriteBitVector(std::vector<u8>& buffer, int bufferSizeBits, u8 bit)
		{
			int posInteger = bufferSizeBits / 8;
			int posFraction = bufferSizeBits % 8;

			if(posFraction == 0)
			{
				buffer.push_back(0);
			}

			buffer.back() |= (bit << (7 - posFraction));

			return bufferSizeBits + 1;
		}

		template <typename T> int WriteBitVector(std::vector<u8>& buffer, int bufferSizeBits, const T& data, int dataSizeBits)
		{
			for(int i = 0; i < dataSizeBits; i++)
			{
				bufferSizeBits = WriteBitVector(buffer, bufferSizeBits, (data >> (dataSizeBits - i - 1)) & 1);
			}

			return bufferSizeBits;
		}

		template <typename ELEMENT_T, int SIZE_BITS> int CompressorRLE<ELEMENT_T, SIZE_BITS>::Compress(const std::vector<ELEMENT_T>& uncompressedData, std::vector<u8>& compressedData)
		{
			//Get uncompressed size
			const int uncompressedSize = uncompressedData.size();

			//Calc max run size
			int maxRunSize = 1;
			for(int i = 0; i < SIZE_BITS; i++)
			{
				maxRunSize += (1 << i);
			}

			//Get element size
			const int elementSizeBits = (sizeof(ELEMENT_T) * 8) - SIZE_BITS;

			//Create data mask
			int dataMask = 0;
			for(int i = 0; i < elementSizeBits; i++)
			{
				dataMask |= (1 << i);
			}

			ELEMENT_T runData;
			u8 runSize = 0;
			int compressedSizeBits = 0;

			for(int i = 0; i < uncompressedSize; i += runSize)
			{
				//Find next run size
				runSize = 0;
				bool duplicate = true;

				//Continue until hit max run size, max u8 size, or element not a duplicate
				for(int j = i; j < uncompressedSize && ((j - i) < maxRunSize) && ((j - i) < 255) && duplicate; j++)
				{
					//Get element
					const ELEMENT_T& element = uncompressedData[j];

					if(runSize == 0)
					{
						//First in run
						runData = element;
						runSize++;
					}
					else
					{
						//If current element is duplicate
						duplicate = (element == runData);

						if(duplicate)
						{
							//Increment size
							runSize++;
						}
					}
				}

				if(runData != 0)
				{
					bool breakMe = true;
				}

				//Sanity check
				debug::Assert((runData & dataMask) == runData, "RLE compression error: element too large, more storage bits required");

				//Set size bits
				runData &= dataMask;
				runData |= ((runSize - 1) << elementSizeBits);

				//Write bytes, little endian
				for(int j = 0; j < sizeof(ELEMENT_T); j++)
				{
					compressedData.push_back(runData >> ((sizeof(ELEMENT_T) - j - 1) * 8) & 0xFF);
				}
			}

			return compressedData.size();
		}

		template <typename ELEMENT_T, int SIZE_BITS> int DecompressorRLE<ELEMENT_T, SIZE_BITS>::Decompress(const std::vector<u8>& compressedData, std::vector<ELEMENT_T>& decompressedData)
		{
			//Get element size
			const int elementSizeBits = (sizeof(ELEMENT_T) * 8) - SIZE_BITS;

			//Create data mask
			int dataMask = 0;
			for(int i = 0; i < elementSizeBits; i++)
			{
				dataMask |= (1 << i);
			}

			for(int i = 0; i < compressedData.size(); i += sizeof(ELEMENT_T))
			{
				//Read bytes, little endian
				ELEMENT_T runData = { 0 };
				
				for(int j = 0; j < sizeof(ELEMENT_T); j++)
				{
					runData |= (compressedData[i + j] << ((sizeof(ELEMENT_T) - j - 1) * 8));
				}

				//Shift size
				u8 runSize = (u8)((runData & ~dataMask) >> elementSizeBits) + 1;

				//Mask data
				runData &= dataMask;

				if(runData != 0)
				{
					bool breakMe = true;
				}

				//Insert data run
				for(int j = 0; j < runSize; j++)
				{
					decompressedData.push_back(runData);
				}
			}

			return decompressedData.size();
		}
	}
}

#endif // Header guard