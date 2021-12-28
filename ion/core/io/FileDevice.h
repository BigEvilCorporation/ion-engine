///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		FileDevice.h
// Date:		20th November 2011
// Authors:		Matt Phillips
// Description:	Interface to a file storage system
///////////////////////////////////////////////////

#pragma once

#include <core/Platform.h>
#include <vector>
#include <string>

namespace ion
{
	namespace io
	{
		//TODO: Pure virtual base class, for cross-platform and device support

		class FileDevice
		{
		public:
			enum class DeviceType { Fixed, Optical, Removable, Network };
			enum class AccessType { ReadOnly, Writeable };
			enum class FileType { File, Directory };

			struct DirectoryItem
			{
				//TODO: Add creation/modify time, etc
				std::string m_filename;
				FileType m_fileType;
				AccessType m_accessType;
			};

			FileDevice(const std::string& label, const std::string& mountPoint, DeviceType deviceType, AccessType accessType);

			bool GetFileExists(const std::string& filename) const;
			bool GetDirectoryExists(const std::string& directory) const;
			s64 GetFileSize(const std::string& filename) const;

			DeviceType GetDeviceType() const;
			AccessType GetAccessType() const;
			const std::string& GetLabel() const;
			const std::string& GetMountPoint() const;
			std::string GetPathSeparator() const;

			void SetDirectory(const std::string& directory);
			const std::string& GetDirectory() const;
			void CreateDirectory(const std::string& path);
			void ReadDirectory(const std::string& directory, std::vector<DirectoryItem>& directoryListing);

			bool Copyfile(const std::string& source, const std::string destination);
			bool DeleteFile(const std::string& path);

			std::string GetFullPath(const std::string& path) const;
			std::string NormalisePath(const std::string& path) const;
			std::string CombinePath(const std::string& path1, const std::string& path2) const;
			void TokenisePath(const std::string& path, std::vector<std::string>& tokens) const;

			static FileDevice* GetDefault();
			static void SetDefault(FileDevice* device);

		private:
			const std::string m_label;
			const std::string m_mountPoint;
			const DeviceType m_deviceType;
			const AccessType m_accessType;

			std::string m_currentDirectory;

			static FileDevice* s_defaultDevice;
		};
	}
}