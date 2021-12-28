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
			debug::Error("FileDevice::GetFileExists() - Not implemented on this platform");
			return false;
		}

		bool FileDevice::GetDirectoryExists(const std::string& directory) const
		{
			debug::Error("FileDevice::GetDirectoryExists() - Not implemented on this platform");
			return false;
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
			debug::Error("FileDevice::CreateDirectory() - Not implemented on this platform");
		}

		void FileDevice::ReadDirectory(const std::string& directory, std::vector<DirectoryItem>& directoryListing)
		{
			debug::Error("FileDevice::ReadDirectory() - Not implemented on this platform");
		}

		bool FileDevice::DeleteFile(const std::string& path)
		{
			debug::Error("FileDevice::DeleteFile() - Not implemented on this platform");
			return false;
		}
	}
}