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
#include "core/io/FileDevice.h"
#include "core/io/File.h"

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

		FileDevice::FileDevice(const std::string& label, const std::string& mountPoint, DeviceType deviceType, AccessType accessType)
			: m_label(label),
			m_mountPoint(mountPoint),
			m_deviceType(deviceType),
			m_accessType(accessType)
		{
			m_currentDirectory = NormalisePath("/");
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

		void FileDevice::SetDirectory(const std::string& directory)
		{
			//If path begins with mount point, skip it
			if(directory.find(m_mountPoint + GetPathSeparator()) == 0)
			{
				m_currentDirectory = directory.substr(0, (m_mountPoint + GetPathSeparator()).size());
			}
			else
			{
				m_currentDirectory = directory;
			}

			m_currentDirectory = NormalisePath(m_currentDirectory);
		}

		const std::string& FileDevice::GetDirectory() const
		{
			return m_currentDirectory;
		}

		bool FileDevice::Copyfile(const std::string& source, const std::string destination)
		{
			std::string srcNormalised = NormalisePath(source);
			std::string dstNormalised = NormalisePath(destination);

			File srcFile;
			File dstFile;

			bool result = false;

			if (srcFile.Open(srcNormalised, File::OpenMode::Read))
			{
				if (dstFile.Open(dstNormalised, File::OpenMode::Write))
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

		std::string FileDevice::GetFullPath(const std::string& path) const
		{
			std::string fullPath = NormalisePath(path);

			//If starts with mount point, presume fully formed path
			if (!ion::string::StartsWith(fullPath, GetMountPoint()))
			{
				std::string directory;

				//If starts with directory, just prepend mount
				if (!ion::string::StartsWith(fullPath, GetDirectory()))
				{
					directory = GetDirectory();
				}

				fullPath = NormalisePath(GetMountPoint() + directory + GetPathSeparator() + fullPath);
			}

			return fullPath;
		}

		std::string FileDevice::NormalisePath(const std::string& inPath) const
		{
			std::string outPath;
			std::string pathSeparator = GetPathSeparator();

			for(unsigned int in = 0; in < inPath.size(); in++)
			{
				if(inPath[in] == '/' || inPath[in] == '\\')
				{
					//Don't copy duplicate slashes
					if (outPath.size() == 0 || !ion::string::StartsWith(outPath.data() + (outPath.size() - 1), pathSeparator))
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

		std::string FileDevice::CombinePath(const std::string& path1, const std::string& path2) const
		{
			return NormalisePath(path1 + GetPathSeparator() + path2);
		}

		void FileDevice::TokenisePath(const std::string& path, std::vector<std::string>& tokens) const
		{
			std::string normalisedPath = NormalisePath(path);
			std::string pathSeparator = GetPathSeparator();
			string::Tokenise(normalisedPath, tokens, pathSeparator);
		}
	}
}
