///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		UUID.cpp
// Date:		2nd May 2016
// Authors:		Matt Phillips
// Description:	Unique Identifier generation
///////////////////////////////////////////////////

#include "core/Platform.h"
#include "core/cryptography/UUID.h"

namespace ion
{
	UUID64 GenerateUUID64()
	{
		ion::Error("GenerateUUID64() - Not implemented on platform");
	}
}