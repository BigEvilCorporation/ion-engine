#include <core/debug/Debug.h>
#include <core/memory/Memory.h>

#include "FileReaderWAV.h"

namespace ion
{
	namespace audio
	{
		FileReaderWAV::FileReaderWAV(const char* filename)
			: FileReaderT(filename)
		{
		}

		FileReaderWAV::~FileReaderWAV()
		{
		}

		bool FileReaderWAV::Open()
		{
			if(m_file.Open(m_filename.c_str(), io::File::OpenMode::Read))
			{
				//Seek to 'RIFF' chunk
				RIFFChunk chunk;
				if(SeekChunk(MakeFourCC("RIFF"), chunk))
				{
					//Read file type
					u32 fileType = 0;
					m_file.Read(&fileType, 4);

					//Read file header
					if(ReadHeader())
					{
						//Seek to data
						SeekRaw(0);

						return true;
					}
				}
				else
				{
					Close();
				}
			}

			return false;
		}

		void FileReaderWAV::Close()
		{
			if(m_file.IsOpen())
				m_file.Close();
		}

		u32 FileReaderWAV::Read(char* data, u32 bytes)
		{
			u32 numBlocks = bytes / m_header.GetBlockSize();
			u32 blockAlignedReadSize = m_header.GetBlockSize() * numBlocks;
			return (u32)m_file.Read(data, blockAlignedReadSize);
		}

		u32 FileReaderWAV::GetPosition()
		{
			return (u32)m_file.GetPosition();
		}

		void FileReaderWAV::SeekRaw(u32 byte)
		{
			m_file.Seek(m_header.m_dataOffset + byte, ion::io::SeekMode::Start);
		}

		void FileReaderWAV::SeekSample(u32 sample)
		{
			SeekRaw(sample * (m_header.GetBitsPerSample() / 8) * m_header.GetNumChannels());
		}

		void FileReaderWAV::SeekTime(float time)
		{
			SeekSample((u32)(time * m_header.GetSampleRate()));
		}

		bool FileReaderWAV::ReadHeader()
		{
			//Seek to 'FMT ' chunk
			RIFFChunk chunk;
			if(SeekChunk(MakeFourCC("fmt "), chunk))
			{
				//Read WAVEFORMATEX
				debug::Assert(chunk.chunkSize <= sizeof(m_header.m_waveHeader), "FileReaderWAV::ReadHeader() - Bad chunk size");
				memory::MemSet(&m_header.m_waveHeader, 0, sizeof(m_header.m_waveHeader));
				m_file.Read(&m_header.m_waveHeader, chunk.chunkSize);

				//Endian flip
				EndianFlipHeader(m_header);

				//Seek to 'DATA' chunk
				if(SeekChunk(MakeFourCC("data"), chunk))
				{
					//Set size
					m_header.m_size = chunk.chunkSize;

					//Set offset
					m_header.m_dataOffset = (u32)m_file.GetPosition();
				}

				return true;
			}

			return false;
		}

		bool FileReaderWAV::SeekChunk(u32 fourCC, RIFFChunk& chunk)
		{
			bool found = false;
			bool endOfFile = false;
			while(!found)
			{
				m_file.Read(&chunk, sizeof(RIFFChunk));
				if(chunk.chunkId == fourCC)
				{
					found = true;
				}
				else
				{
					endOfFile = (m_file.Seek(chunk.chunkSize) == (m_file.GetSize() - 1));
				}
			}

			return found;
		}

		u32 FileReaderWAV::MakeFourCC(const char* string)
		{
			return (u32)(((string[3])<<24) | ((string[2])<<16) | ((string[1])<<8) | (string[0]));
		}

		void FileReaderWAV::EndianFlipHeader(FileHeaderWAV& header)
		{
			//TODO: Implement
		}
	}
}
