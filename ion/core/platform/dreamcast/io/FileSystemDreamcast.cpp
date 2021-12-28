///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		FileSystem.cpp
// Date:		20th November 2011
// Authors:		Matt Phillips
// Description:	File device management
///////////////////////////////////////////////////

#include "core/debug/Debug.h"
#include "core/Platform.h"
#include "core/string/String.h"
#include "core/io/FileSystem.h"

namespace ion
{
	namespace io
	{
		void FileSystem::EnumerateDevices()
		{
			#warning "Need to implement FileSystem::EnumerateDevices()"
		}

		std::string FileSystem::GetUserDataDirectory()
		{
			return std::string("");
		}
	}
}