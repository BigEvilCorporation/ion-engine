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
#include "io/FileSystem.h"

#if defined ION_PLATFORM_WINDOWS
#include <shlobj.h>
#endif

namespace ion
{
	namespace io
	{
		FileSystem::FileSystem()
		{
			m_defaultFileDevice = NULL;

			//Enumerate all file devices
			EnumerateDevices();
		}

		FileSystem::~FileSystem()
		{
			//Cleanup file devices
			for(unsigned int i = 0; i < m_fileDevices.size(); i++)
			{
				delete m_fileDevices[i];
			}
		}

		void FileSystem::EnumerateDevices()
		{
#if defined ION_PLATFORM_WINDOWS
			static const int stringBufferLength = 2048;
			char stringBuffer[stringBufferLength] = {0};

			//Get current working directory
			_getcwd(stringBuffer, stringBufferLength);
			std::string workingDirectory = stringBuffer;

			//Get root path for working directory
			std::string workingRoot = workingDirectory.substr(0, 3);

			//Suppress "Drive not ready" error dialogue when getting attributes for floppy drives
			SetErrorMode(SEM_FAILCRITICALERRORS);

			//Enumerate volumes
			bool volumeFound = true;
			HANDLE volumeHndl = INVALID_HANDLE_VALUE;
			for(volumeHndl = FindFirstVolume(stringBuffer, stringBufferLength);
				volumeHndl != INVALID_HANDLE_VALUE && volumeFound;
				volumeFound = (FindNextVolume(volumeHndl, stringBuffer, stringBufferLength) != 0))
			{
				std::string volumeName = stringBuffer;

				//Get device type
				unsigned int winDriveType = GetDriveType(volumeName.c_str());
				FileDevice::DeviceType deviceType = FileDevice::eFixed;

				bool deviceSupported = true;

				switch(winDriveType)
				{
				case DRIVE_REMOVABLE:
					deviceType = FileDevice::eRemovable;
					break;

				case DRIVE_FIXED:
					deviceType = FileDevice::eFixed;
					break;

				case DRIVE_REMOTE:
					deviceType = FileDevice::eNetwork;
					break;

				case DRIVE_CDROM:
					deviceType = FileDevice::eOptical;
					break;

				default:
					//Unsupported device
					deviceSupported = false;
					break;
				}

				if(deviceSupported)
				{
					int returnLength = 0;

					//Get Windows root path name for device
					GetVolumePathNamesForVolumeName(volumeName.c_str(), stringBuffer, stringBufferLength, (PDWORD)&returnLength);
					std::string rootPath = stringBuffer;

					if(returnLength > 1)
					{
						//Check if path is writeable
						DWORD folderAttributes = GetFileAttributes((LPCSTR)rootPath.c_str());

						FileDevice::AccessType accessType = (folderAttributes & FILE_ATTRIBUTE_READONLY) ? FileDevice::eReadOnly : FileDevice::eWriteable;

						//Create device
						FileDevice* device = new FileDevice(rootPath, rootPath, deviceType, accessType);

						//Add device
						m_fileDevices.push_back(device);

						//Check if the current working directory is on this volume
						if(!_stricmp(rootPath.c_str(), workingRoot.c_str()))
						{
							//Set as default device
							SetDefaultFileDevice(*device);

							//Set current directory
							std::string directory = workingDirectory.substr(rootPath.size(), workingDirectory.size() - rootPath.size());
							device->SetDirectory(directory);
						}
					}
				}
			}

			//Finished volume search
			FindVolumeClose(volumeHndl);

			//Restore default error mode
			SetErrorMode(0);
#elif defined ION_PLATFORM_LINUX
#warning "Need to implement FileSystem::EnumerateDevices()"
			FileDevice* device = new FileDevice("/", "/", FileDevice::eFixed, FileDevice::eWriteable);
			m_fileDevices.push_back(device);
			SetDefaultFileDevice(*device);
#elif defined ION_PLATFORM_MACOSX
#warning "Need to implement FileSystem::EnumerateDevices()"
            FileDevice* device = new FileDevice("/", "/", FileDevice::eFixed, FileDevice::eWriteable);
            m_fileDevices.push_back(device);
            SetDefaultFileDevice(*device);
#endif
		}

		FileDevice* FileSystem::FindFileDevice(io::FileDevice::DeviceType, io::FileDevice::AccessType accessType)
		{
			return NULL;
		}

		FileDevice* FileSystem::FindFileDevice(std::string label)
		{
			return NULL;
		}

		void FileSystem::SetDefaultFileDevice(FileDevice& device)
		{
			m_defaultFileDevice = &device;
		}

		FileDevice* FileSystem::GetDefaultFileDevice()
		{
			return m_defaultFileDevice;
		}

		std::string FileSystem::GetUserDataDirectory()
		{
#if defined ION_PLATFORM_WINDOWS
			WCHAR* directory = nullptr;
			SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &directory);
			std::wstring temp(directory);
			std::string directoryA(temp.begin(), temp.end());
			CoTaskMemFree(directory);
			return directoryA;
#elif defined ION_PLATFORM_LINUX
			char *home = ::getenv("HOME");
			if (home == NULL)
				ion::debug::Error("GetUserDataDirectory: no home directory defined! ($HOME is missing)");
			else if (*home == '\0')
				ion::debug::Error("GetUserDataDirectory: no home directory defined! ($HOME is empty)");
			return std::string(home);
#else
			return std::string("");
#endif
		}
	}
}
