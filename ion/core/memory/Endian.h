///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Endian.h
// Date:		10th January 2017
// Authors:		Matt Phillips
// Description:	Endian swapping and detection
///////////////////////////////////////////////////

#pragma once

#include "core/Types.h"

namespace ion
{
	namespace memory
	{
		inline void EndianSwap(u16& value);
		inline void EndianSwap(s16& value);
		inline void EndianSwap(u32& value);
		inline void EndianSwap(s32& value);
	}
}

namespace ion
{
	namespace memory
	{
		void EndianSwap(u16& value)
		{
			u8* bytes = (u8*)&value;
			u8 temp = bytes[0];
			bytes[0] = bytes[1];
			bytes[1] = temp;
		}

		void EndianSwap(s16& value)
		{
			u8* bytes = (u8*)&value;
			u8 temp = bytes[0];
			bytes[0] = bytes[1];
			bytes[1] = temp;
		}

		void EndianSwap(u32& value)
		{
			u8* bytes = (u8*)&value;
			u8 temp = bytes[0];
			bytes[0] = bytes[3];
			bytes[3] = temp;
			temp = bytes[1];
			bytes[1] = bytes[2];
			bytes[2] = temp;
		}

		void EndianSwap(s32& value)
		{
			u8* bytes = (u8*)&value;
			u8 temp = bytes[0];
			bytes[0] = bytes[3];
			bytes[3] = temp;
			temp = bytes[1];
			bytes[1] = bytes[2];
			bytes[2] = temp;
		}
	}
}