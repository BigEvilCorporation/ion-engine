///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Types.h
// Date:		18th July 2018
// Authors:		Matt Phillips
// Description:	STL helper utilities
///////////////////////////////////////////////////

#pragma once

#include <vector>
#include <algorithm>
#include <iomanip>

//std::stringstream modifier helpers
#define SSTREAM_HEX1(val) std::hex << std::setfill('0') << std::setw(1) << std::uppercase << (int)(val&0xF) << std::dec
#define SSTREAM_HEX2(val) std::hex << std::setfill('0') << std::setw(2) << std::uppercase << (int)(val&0xFF) << std::dec
#define SSTREAM_HEX4(val) std::hex << std::setfill('0') << std::setw(4) << std::uppercase << (int)(val&0xFFFF) << std::dec
#define SSTREAM_HEX8(val) std::hex << std::setfill('0') << std::setw(8) << std::uppercase << (int)(val&0xFFFFFFFF) << std::dec

#define SSTREAM_INT2(val) std::dec << std::setfill('0') << std::setw(2) << (int)val
#define SSTREAM_INT4(val) std::dec << std::setfill('0') << std::setw(4) << (int)val

namespace ion
{
	namespace utils
	{
		namespace stl
		{
			template<typename T> bool Find(const std::vector<T>& vector, const T& item)
			{
				return std::find(vector.begin(), vector.end(), item) != vector.end();
			}

			template<typename T> void FindAndRemove(std::vector<T>& vector, const T& item)
			{
				vector.erase(std::remove(vector.begin(), vector.end(), item), vector.end());
			}

			template<typename T> int IndexOf(const std::vector<T>& vector, const T& item)
			{
				int index = -1;

				typename std::vector<T>::const_iterator it = std::find(vector.begin(), vector.end(), item);
				if (it != vector.end())
				{
					index = (int)std::distance(vector.begin(), it);
				}

				return index;
			}
		}
	}
}