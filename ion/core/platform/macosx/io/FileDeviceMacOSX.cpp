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

#include <sys/stat.h>

namespace ion
{
	namespace io
	{
		bool FileDevice::GetFileExists(const std::string& filename) const
		{
			std::string normalisedPath = NormalisePath(filename);

			struct stat fileInfo;
			if (stat(normalisedPath.c_str(), &fileInfo) != 0)
				return false;
			if (!S_ISREG(fileInfo.st_mode))
				return false;
			return true;

			return true;
		}

		bool FileDevice::GetDirectoryExists(const std::string& directory) const
		{
			std::string normalisedPath = NormalisePath(directory);

			struct stat dirInfo;
			if (stat(normalisedPath.c_str(), &dirInfo) != 0)
				return false;
			if (!S_ISDIR(dirInfo.st_mode))
				return false;
			return true;

			return true;
		}

		s64 FileDevice::GetFileSize(const std::string& filename) const
		{
			debug::Error("FileDevice::GetFileSize() - Not implemented on this platform");
			return 0;
		}

		std::string FileDevice::GetPathSeparator() const
		{
			return "/";
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
					mkdir(currentPath.c_str(), 0775);
				}

				currentPath += pathSeparator;
			}
		}

		void FileDevice::ReadDirectory(const std::string& directory, std::vector<DirectoryItem>& directoryListing)
		{
			debug::Error("FileDevice::ReadDirectory() - Not implemented on this platform");
		}

		bool FileDevice::DeleteFile(const std::string& path)
		{
			std::string normalisedPath = NormalisePath(path);
			return remove(normalisedPath.c_str()) == 0;
		}
	}
}