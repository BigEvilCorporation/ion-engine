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
#include "core/io/FileDevice.h"
#include "core/io/File.h"

namespace ion
{
	namespace io
	{
		bool FileDevice::GetFileExists(const std::string& filename) const
		{
			std::string normalisedPath = NormalisePath(filename);

			WIN32_FIND_DATA findFileData;
			HANDLE handle = FindFirstFile(string::StringToNative(normalisedPath).c_str(), &findFileData);

			bool found = (handle != INVALID_HANDLE_VALUE);
			if (found)
			{
				FindClose(handle);
			}

			return found;
		}

		bool FileDevice::GetDirectoryExists(const std::string& directory) const
		{
			std::string normalisedPath = NormalisePath(directory);
			DWORD attrib = GetFileAttributes(string::StringToNative(normalisedPath).c_str());
			return ((attrib != INVALID_FILE_ATTRIBUTES) && (attrib & FILE_ATTRIBUTE_DIRECTORY));
		}

		s64 FileDevice::GetFileSize(const std::string& filename) const
		{
			HANDLE hndl = CreateFile(string::StringToNative(filename).c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

			if (hndl == INVALID_HANDLE_VALUE)
				return 0;

			LARGE_INTEGER size = { 0 };
			GetFileSizeEx(hndl, &size);
			CloseHandle(hndl);

			return size.QuadPart;
		}

		std::string FileDevice::GetPathSeparator() const
		{
			return "\\";
		}

		void FileDevice::CreateDirectory(const std::string& path)
		{
			std::vector<std::string> tokenisedPath;
			TokenisePath(path, tokenisedPath);
			std::string pathSeparator = GetPathSeparator();

			std::string currentPath("");

			for (int i = 0; i < tokenisedPath.size(); i++)
			{
				currentPath += tokenisedPath[i];

				if (!GetDirectoryExists(currentPath))
				{
#if UNICODE
					CreateDirectoryW(string::StringToNative(currentPath).c_str(), NULL);
#else
					CreateDirectoryA(string::StringToNative(currentPath).c_str(), NULL);
#endif
				}

				currentPath += pathSeparator;
			}
		}

		void FileDevice::ReadDirectory(const std::string& directory, std::vector<DirectoryItem>& directoryListing)
		{
			HANDLE searchHndl = INVALID_HANDLE_VALUE;
			bool fileFound = true;

			WIN32_FIND_DATA findData = { 0 };

			string_t path;

			if (!ion::string::StartsWith(directory, m_mountPoint))
			{
				path += string::StringToNative(m_mountPoint);
			}

			path += string::StringToNative(directory);
			path += string::StringToNative("\\*.*");
			path = string::StringToNative(NormalisePath(string::NativeToString(path)));

			for (searchHndl = FindFirstFile(path.c_str(), &findData);
				searchHndl != INVALID_HANDLE_VALUE && fileFound;
				fileFound = (FindNextFile(searchHndl, &findData) != 0))
			{
				DirectoryItem directoryItem;

				directoryItem.m_filename = string::NativeToString(findData.cFileName);
				directoryItem.m_fileType = (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? FileType::Directory : FileType::File;
				directoryItem.m_accessType = (findData.dwFileAttributes & FILE_ATTRIBUTE_READONLY) ? AccessType::ReadOnly : AccessType::Writeable;

				directoryListing.push_back(directoryItem);
			}
		}

		bool FileDevice::DeleteFile(const std::string& path)
		{
			std::string normalisedPath = NormalisePath(path);
			return ::DeleteFileA(normalisedPath.c_str()) != 0;
		}
	}
}