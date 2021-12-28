///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		FileSystem.h
// Date:		20th November 2011
// Authors:		Matt Phillips
// Description:	File device management
///////////////////////////////////////////////////

#pragma once

#include "core/io/FileDevice.h"

#include <vector>
#include <string>

namespace ion
{
	namespace io
	{
		class FileSystem
		{
		public:

			FileSystem();
			virtual ~FileSystem();

			//Find file device
			static FileDevice* FindFileDevice(io::FileDevice::DeviceType deviceType, io::FileDevice::AccessType accessType);
			static FileDevice* FindFileDevice(const std::string& path);

			//Set default file device
			void SetDefaultFileDevice(FileDevice* device);
			FileDevice* GetDefaultFileDevice();

			//Get user data directory (e.g., "C:\[user]\appdata\roaming" on Windows)
			std::string GetUserDataDirectory();

		private:
			void EnumerateDevices();
			static std::vector<FileDevice*> s_fileDevices;
			static FileDevice* s_defaultFileDevice;
		};
	}
}