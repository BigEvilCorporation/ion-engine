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
			enum DeviceType { eFixed, eOptical, eRemovable, eNetwork };
			enum AccessType { eReadOnly, eWriteable };
			enum FileType { eFile, eDirectory };

			struct DirectoryItem
			{
				//TODO: Add creation/modify time, etc
				std::string m_filename;
				FileType m_fileType;
				AccessType m_accessType;
			};

			FileDevice(std::string label, std::string mountPoint, DeviceType deviceType, AccessType accessType);

			bool GetFileExists(const std::string& filename) const;
			bool GetDirectoryExists(const std::string& directory) const;

			DeviceType GetDeviceType() const;
			AccessType GetAccessType() const;
			const std::string& GetLabel() const;
			const std::string& GetMountPoint() const;
			char GetPathSeparator() const;

			void SetDirectory(std::string directory);
			const std::string& GetDirectory() const;
			void CreateDirectory(const std::string path);
			void ReadDirectory(std::string directory, std::vector<DirectoryItem>& directoryListing);

			bool Copyfile(const std::string& source, const std::string destination);
			bool DeleteFile(const std::string& path);

			std::string NormalisePath(std::string path) const;
			void TokenisePath(std::string path, std::vector<std::string>& tokens) const;

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