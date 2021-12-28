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
			FileDevice* device = new FileDevice("/", "/", FileDevice::eFixed, FileDevice::eWriteable);
			s_fileDevices.push_back(device);
			SetDefaultFileDevice(device);
		}

		std::string FileSystem::GetUserDataDirectory()
		{
			char* home = ::getenv("HOME");

			if (home == NULL)
				ion::debug::Error("GetUserDataDirectory: no home directory defined! ($HOME is missing)");
			else if (*home == '\0')
				ion::debug::Error("GetUserDataDirectory: no home directory defined! ($HOME is empty)");

			return std::string(home);
		}
	}
}