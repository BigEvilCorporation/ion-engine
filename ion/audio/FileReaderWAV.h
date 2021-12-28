#pragma once

#include <audio/DataFormat.h>
#include <audio/FileReader.h>

namespace ion
{
	namespace audio
	{
		struct RIFFChunk
		{
			u32 chunkId;
			u32 chunkSize;
		};

		struct FormatID
		{
			unsigned long  data1;
			unsigned short data2;
			unsigned short data3;
			char           data4[8];
		};

		struct WaveHeader
		{
			u16 format;			//Format type
			u16 numChannels;	//Number of channels
			u32 samplesPerSec;	//Sample rate
			u32 bytesPerSec;	//Playback rate
			u16 blockSize;		//Block size
			u16 bitsPerSample;	//Bits per sample (per channel)
			u16 extSize;		//Extended struct size (bytes below this)
			u16 samplesPerBlock;//Samples per block
			u32 channelMask;	//Present channel mask
			FormatID subFormat;	//SUbformat ID
		};

		class FileHeaderWAV : public FileHeader
		{
		public:
			FileHeaderWAV() {}

			virtual DataFormat GetEncodedFormat() const { return DataFormat::PCM16; }
			virtual DataFormat GetDecodedFormat() const { return DataFormat::PCM16; }
			virtual u32 GetNumChannels() const { return m_waveHeader.numChannels; }
			virtual u32 GetSampleRate() const { return m_waveHeader.samplesPerSec; }
			virtual u32 GetBitsPerSample() const { return m_waveHeader.bitsPerSample; }
			virtual u32 GetBlockSize() const { return m_waveHeader.blockSize; }
			virtual u32 GetEncodedSizeBytes() const { return m_size; }
			virtual u32 GetDecodedSizeBytes() const { return m_size; }
			virtual u32 GetSizeSamples() const { return m_size / (GetBitsPerSample() / 8) / GetNumChannels(); }

			WaveHeader m_waveHeader;
			u32 m_size;
			u32 m_dataOffset;
		};

		class FileReaderWAV : public FileReaderT<FileHeaderWAV>
		{
		public:
			FileReaderWAV(const char* filename);
			virtual ~FileReaderWAV();

			virtual bool Open();
			virtual void Close();

			virtual u32 Read(char* data, u32 bytes);

			virtual u32 GetPosition();

			virtual void SeekRaw(u32 byte);
			virtual void SeekSample(u32 sample);
			virtual void SeekTime(float time);

		protected:
			virtual bool ReadHeader();
			bool SeekChunk(u32 fourCC, RIFFChunk& chunk);

			static u32 MakeFourCC(const char* string);
			static void EndianFlipHeader(FileHeaderWAV& header);
		};
	}
}
