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

#include <shlobj.h>

namespace ion
{
	namespace io
	{
		void FileSystem::EnumerateDevices()
		{
			static const int stringBufferLength = 2048;
			char_t stringBuffer[stringBufferLength] = { 0 };

			//Get current working directory
#if UNICODE
			_wgetcwd(stringBuffer, stringBufferLength);
#else
			_getcwd(stringBuffer, stringBufferLength);
#endif
			string_t workingDirectory = stringBuffer;

			//Get root path for working directory
			string_t workingRoot = workingDirectory.substr(0, 3);

			//Suppress "Drive not ready" error dialogue when getting attributes for floppy drives
			SetErrorMode(SEM_FAILCRITICALERRORS);

			//Enumerate volumes
			bool volumeFound = true;
			HANDLE volumeHndl = INVALID_HANDLE_VALUE;
			for (volumeHndl = FindFirstVolume(stringBuffer, stringBufferLength);
				volumeHndl != INVALID_HANDLE_VALUE && volumeFound;
				volumeFound = (FindNextVolume(volumeHndl, stringBuffer, stringBufferLength) != 0))
			{
				string_t volumeName = stringBuffer;

				//Get device type
				unsigned int winDriveType = GetDriveType(stringBuffer);
				FileDevice::DeviceType deviceType = FileDevice::DeviceType::Fixed;

				bool deviceSupported = true;

				switch (winDriveType)
				{
				case DRIVE_REMOVABLE:
					deviceType = FileDevice::DeviceType::Removable;
					break;

				case DRIVE_FIXED:
					deviceType = FileDevice::DeviceType::Fixed;
					break;

				case DRIVE_REMOTE:
					deviceType = FileDevice::DeviceType::Network;
					break;

				case DRIVE_CDROM:
					deviceType = FileDevice::DeviceType::Optical;
					break;

				default:
					//Unsupported device
					deviceSupported = false;
					break;
				}

				if (deviceSupported)
				{
					int returnLength = 0;

					//Get Windows root path name for device
					GetVolumePathNamesForVolumeName(volumeName.c_str(), stringBuffer, stringBufferLength, (PDWORD)&returnLength);
					string_t rootPath = stringBuffer;

					if (returnLength > 1)
					{
						//Check if path is writeable
						DWORD folderAttributes = GetFileAttributes(rootPath.c_str());

						FileDevice::AccessType accessType = (folderAttributes & FILE_ATTRIBUTE_READONLY) ? FileDevice::AccessType::ReadOnly : FileDevice::AccessType::Writeable;

						//Create device
						FileDevice* device = new FileDevice(string::NativeToString(rootPath), string::NativeToString(rootPath), deviceType, accessType);

						//Add device
						s_fileDevices.push_back(device);

						//Check if the current working directory is on this volume
						if (string::CompareNoCase(rootPath.c_str(), workingRoot.c_str()))
						{
							//Set as default device
							SetDefaultFileDevice(device);

							//Set current directory
							std::string directory = string::NativeToString(workingDirectory.substr(rootPath.size(), workingDirectory.size() - rootPath.size()));
							device->SetDirectory(directory);
						}
					}
				}
			}

			//Finished volume search
			FindVolumeClose(volumeHndl);

			//Restore default error mode
			SetErrorMode(0);
		}

		std::string FileSystem::GetUserDataDirectory()
		{
			WCHAR* directory = nullptr;
			SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &directory);
			std::wstring temp(directory);
			std::string directoryA(temp.begin(), temp.end());
			CoTaskMemFree(directory);
			return directoryA;
		}
	}
}