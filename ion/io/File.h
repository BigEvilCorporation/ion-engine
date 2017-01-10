/*
	Symmetry Engine 2.0
	Matt Phillips (c) 2010

	File.h

	File IO
*/

#pragma once

#include "core/Types.h"
#include "io/Stream.h"

#include <fstream>
#include <string>

namespace ion
{
	namespace io
	{
		bool FileExists(const std::string& filename);
		s64 GetFileSize(const std::string& filename);

		class File : public io::Stream
		{
			static const int s_bufferSize = (1024 * 256);

		public:
			enum OpenMode { eOpenRead, eOpenWrite, eOpenAppend };

			File();
			File(const std::string& filename, OpenMode openMode = eOpenRead);
			virtual ~File();

			virtual bool Open(const std::string& filename, OpenMode openMode = eOpenRead);
			virtual void Close();

			virtual s64 GetSize() const;
			virtual s64 GetPosition() const;
			virtual s64 Read(void* data, s64 size);
			virtual s64 Write(const void* data, s64 Size);
			virtual s64 Seek(s64 position, SeekMode origin = eSeekModeCurrent);

			void Flush();

			bool IsOpen() const;

			OpenMode GetOpenMode() const;

		protected:
			void FillBuffer(s64 position);

			std::string m_filename;
			OpenMode m_openMode;

			s64 m_currentPosition;
			s64 m_size;
			s64 m_readBufferStart;
			s64 m_readBufferEnd;

			bool m_open;

			std::fstream m_stream;
			u8 m_readBuffer[s_bufferSize];
		};
	}
} //Namespace