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
		std::vector<FileDevice*> FileSystem::s_fileDevices;
		FileDevice* FileSystem::s_defaultFileDevice = nullptr;

		FileSystem::FileSystem()
		{
			//Enumerate all file devices
			EnumerateDevices();
		}

		FileSystem::~FileSystem()
		{
			//Cleanup file devices
			for(unsigned int i = 0; i < s_fileDevices.size(); i++)
			{
				delete s_fileDevices[i];
			}
		}

		FileDevice* FileSystem::FindFileDevice(io::FileDevice::DeviceType deviceType, io::FileDevice::AccessType accessType)
		{
			for (int i = 0; i < s_fileDevices.size(); i++)
			{
				if (s_fileDevices[i]->GetDeviceType() == deviceType && s_fileDevices[i]->GetAccessType() == accessType)
				{
					return s_fileDevices[i];
				}
			}

			return nullptr;
		}

		FileDevice* FileSystem::FindFileDevice(const std::string& path)
		{
			for (int i = 0; i < s_fileDevices.size(); i++)
			{
				std::string fullPath = s_fileDevices[i]->NormalisePath(path);

				if (ion::string::StartsWith(fullPath, s_fileDevices[i]->GetMountPoint()))
				{
					return s_fileDevices[i];
				}
			}

			return nullptr;
		}

		void FileSystem::SetDefaultFileDevice(FileDevice* device)
		{
			FileDevice::SetDefault(device);
			s_defaultFileDevice = device;
		}

		FileDevice* FileSystem::GetDefaultFileDevice()
		{
			return s_defaultFileDevice;
		}
	}
}
