#pragma once

#include <audio/StreamDesc.h>
#include <core/io/File.h>
#include <string>

namespace ion
{
	namespace audio
	{
		class FileHeader : public StreamDesc
		{
		public:
			FileHeader() {}
		};

		class FileReader
		{
		public:
			FileReader(const char* filename);
			virtual ~FileReader();

			virtual bool Open() = 0;
			virtual void Close() = 0;

			virtual u32 Read(char* data, u32 bytes) = 0;

			virtual u32 GetPosition() = 0;

			virtual void SeekRaw(u32 byte) = 0;
			virtual void SeekSample(u32 sample) = 0;
			virtual void SeekTime(float time) = 0;

			virtual const StreamDesc& GetStreamDesc() const = 0;

			const std::string& GetFilename() { return m_filename; }

		protected:
			virtual bool ReadHeader() = 0;

			std::string m_filename;
			io::File m_file;
		};

		template <class HeaderT> class FileReaderT : public FileReader
		{
		public:
			FileReaderT(const char* filename) : FileReader(filename) {}
			virtual const StreamDesc& GetStreamDesc() const { return m_header; }
		protected:
			HeaderT m_header;
		};
	}
}