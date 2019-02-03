///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		FileDevice.cpp
// Date:		20th November 2011
// Authors:		Matt Phillips
// Description:	Interface to a file storage system
///////////////////////////////////////////////////

#include "core/Platform.h"
#include "core/string/String.h"
#include "core/debug/Debug.h"
#include "maths/Maths.h"
#include "io/FileDevice.h"
#include "io/File.h"

#if defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX
#include <sys/stat.h>
#endif

namespace ion
{
	namespace io
	{
		FileDevice* FileDevice::s_defaultDevice = nullptr;

		FileDevice* FileDevice::GetDefault()
		{
			return s_defaultDevice;
		}

		void FileDevice::SetDefault(FileDevice* device)
		{
			s_defaultDevice = device;
		}

		FileDevice::FileDevice(std::string label, std::string mountPoint, DeviceType deviceType, AccessType accessType)
			: m_label(label),
			m_mountPoint(mountPoint),
			m_deviceType(deviceType),
			m_accessType(accessType)
		{
			m_currentDirectory = "/";
		}

		bool FileDevice::GetFileExists(const std::string& filename) const
		{
			std::string normalisedPath = NormalisePath(filename);

#if defined ION_PLATFORM_WINDOWS
			WIN32_FIND_DATA findFileData;
			HANDLE handle = FindFirstFile(normalisedPath.c_str(), &findFileData);

			bool found = (handle != INVALID_HANDLE_VALUE);
			if (found)
			{
				FindClose(handle);
			}

			return found;
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX
			struct stat fileInfo;
			if (stat(normalisedPath.c_str(), &fileInfo) != 0)
				return false;
			if (!S_ISREG(fileInfo.st_mode))
				return false;
			return true;
#else
			debug::Error("FileDevice::GetFileExists() - Not implemented on this platform");
			return false;
#endif
		}

		bool FileDevice::GetDirectoryExists(const std::string& directory) const
		{
			std::string normalisedPath = NormalisePath(directory);

#if defined ION_PLATFORM_WINDOWS
			DWORD attrib = GetFileAttributes(normalisedPath.c_str());

			return ((attrib != INVALID_FILE_ATTRIBUTES) && (attrib & FILE_ATTRIBUTE_DIRECTORY));
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX
			struct stat dirInfo;
			if (stat(normalisedPath.c_str(), &dirInfo) != 0)
				return false;
			if (!S_ISDIR(dirInfo.st_mode))
				return false;
			return true;
#else
            debug::Error("FileDevice::GetDirectoryExists() - Not implemented on this platform");
			return false;
#endif
		}

		FileDevice::DeviceType FileDevice::GetDeviceType() const
		{
			return m_deviceType;
		}

		FileDevice::AccessType FileDevice::GetAccessType() const
		{
			return m_accessType;
		}

		const std::string& FileDevice::GetLabel() const
		{
			return m_label;
		}

		const std::string& FileDevice::GetMountPoint() const
		{
			return m_mountPoint;
		}

		char FileDevice::GetPathSeparator() const
		{
#if defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX || defined ION_PLATFORM_DREAMCAST
			return '/';
#else
			return '\\';
#endif
		}

		void FileDevice::SetDirectory(std::string directory)
		{
			//If path begins with mount point, skip it
			if(directory.find(m_mountPoint) == 0)
			{
				m_currentDirectory = directory.substr(0, m_mountPoint.size());
			}
			else
			{
				m_currentDirectory = NormalisePath(directory);
			}
		}

		const std::string& FileDevice::GetDirectory() const
		{
			return m_currentDirectory;
		}

		void FileDevice::CreateDirectory(const std::string path)
		{
			std::vector<std::string> tokenisedPath;
			TokenisePath(path, tokenisedPath);
			char pathSeparator = GetPathSeparator();

			std::string currentPath("");

			for (int i = 0; i < tokenisedPath.size(); i++)
			{
				currentPath += tokenisedPath[i];

				if (!GetDirectoryExists(currentPath))
				{
#if defined ION_PLATFORM_WINDOWS
					CreateDirectoryA(currentPath.c_str(), NULL);
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX
					mkdir(currentPath.c_str(), 0775);
#else
					debug::Error("FileDevice::CreateDirectory() - Not implemented on this platform");
#endif
				}

				currentPath += pathSeparator;
			}
		}

		void FileDevice::ReadDirectory(std::string directory, std::vector<DirectoryItem>& directoryListing)
		{
#if defined ION_PLATFORM_WINDOWS
			HANDLE searchHndl = INVALID_HANDLE_VALUE;
			bool fileFound = true;

			WIN32_FIND_DATA findData = {0};

			std::string path = m_mountPoint;
			path += directory;
			path += "\\*.*";
			path = NormalisePath(path);

			for(searchHndl = FindFirstFile(path.c_str(), &findData);
				searchHndl != INVALID_HANDLE_VALUE && fileFound;
				fileFound = (FindNextFile(searchHndl, &findData) != 0))
			{
				DirectoryItem directoryItem;

				directoryItem.m_filename = findData.cFileName;
				directoryItem.m_fileType = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? eDirectory : eFile;
				directoryItem.m_accessType = (findData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? eReadOnly : eWriteable;

				directoryListing.push_back(directoryItem);
			}
#else
			debug::Error("FileDevice::ReadDirectory() - Not implemented on this platform");
#endif
		}

		bool FileDevice::Copyfile(const std::string& source, const std::string destination)
		{
			std::string srcNormalised = NormalisePath(source);
			std::string dstNormalised = NormalisePath(destination);

			File srcFile;
			File dstFile;

			bool result = false;

			if (srcFile.Open(srcNormalised, File::eOpenRead))
			{
				if (dstFile.Open(dstNormalised, File::eOpenWrite))
				{
					const s64 bufferSize = 1024 * 8;
					char buffer[bufferSize] = { 0 };

					s64 bytesRemaining = srcFile.GetSize();

					result = true;

					while (bytesRemaining && result)
					{
						s64 bytesToRead = maths::Min(bytesRemaining, bufferSize);
						s64 bytesRead = srcFile.Read(buffer, bytesToRead);

						if (bytesToRead != bytesRead)
						{
							result = false;
						}

						s64 bytesWritten = dstFile.Write(buffer, bytesRead);

						if (bytesWritten != bytesRead)
						{
							result = false;
						}

						bytesRemaining -= bytesRead;
					}

					dstFile.Close();
				}

				srcFile.Close();
			}

			return result;
		}

		bool FileDevice::DeleteFile(const std::string& path)
		{
			std::string normalisedPath = NormalisePath(path);

#if defined ION_PLATFORM_WINDOWS
			return ::DeleteFileA(normalisedPath.c_str()) != 0;
#elif defined ION_PLATFORM_LINUX || defined ION_PLATFORM_MACOSX
            return remove(normalisedPath.c_str()) == 0;
#else
			debug::Error("FileDevice::DeleteFile() - Not implemented on this platform");
			return false;
#endif
		}

		std::string FileDevice::NormalisePath(std::string inPath) const
		{
			std::string outPath;
			char pathSeparator = GetPathSeparator();

			for(unsigned int in = 0; in < inPath.size(); in++)
			{
				if(inPath[in] == '/' || inPath[in] == '\\')
				{
					//Don't copy duplicate slashes
					if (outPath.size() == 0 || outPath[outPath.size() - 1] != pathSeparator)
					{
						outPath += pathSeparator;
					}
				}
				else
				{
					outPath += inPath[in];
				}
			}

			return outPath;
		}

		void FileDevice::TokenisePath(std::string path, std::vector<std::string>& tokens) const
		{
			std::string normalisedPath = NormalisePath(path);
			char pathSeparator = GetPathSeparator();
			string::Tokenise(normalisedPath, tokens, pathSeparator);
		}
	}
}
