///////////////////////////////////////////////////
// (c) 2018 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Fixed.h
// Date:		20th December 2018
// Authors:		Matt Phillips
// Description:	Fixed point number conversions
///////////////////////////////////////////////////

#pragma once

#include <ion/core/Types.h>

namespace ion
{
	namespace maths
	{
		float Fixed1616ToFloat(s32 number);
		s32 FloatToFixed1616(float number);
	}
}