///////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Fixed.cpp
// Date:		20th December 2018
// Authors:		Matt Phillips
// Description:	Fixed point number conversions
///////////////////////////////////////////////////

#include "Fixed.h"

namespace ion
{
	namespace maths
	{
		float Fixed1616ToFloat(s32 number)
		{
			return ((float)number) / 65536.0f;
		}

		s32 FloatToFixed1616(float number)
		{
			return (s32)(number * 65536.0f);
		}
	}
}