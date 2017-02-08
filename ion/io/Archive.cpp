///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Archive.cpp
// Date:		22nd November 2012
// Authors:		Matt Phillips
// Description:	Serialisable archive
///////////////////////////////////////////////////

#include "io/Archive.h"
#include "io/Stream.h"
#include "core/cryptography/Hash.h"

namespace ion
{
	namespace io
	{
		Archive::Archive(Stream& stream, Direction direction, ResourceManager* resourceManager, u32 version)
			: m_stream(stream)
		{
			m_direction = direction;
			m_version = version;
			m_resourceManager = resourceManager;
		}

		void Archive::Serialise(void* data, u64 size)
		{
			if(GetDirection() == eIn)
			{
				m_stream.Read(data, size);
			}
			else
			{
				if(m_blockStack.size() > 0)
				{
					//Not root block, write to temp data
					m_blockStack.back().data.Write(data, size);
				}
				else
				{
					//Root block, write to file
					m_stream.Write(data, size);
				}
			}
		}

		Archive::Direction Archive::GetDirection() const
		{
			return m_direction;
		}

		u32 Archive::GetVersion() const
		{
			return m_version;
		}

		ResourceManager* Archive::GetResourceManager() const
		{
			return m_resourceManager;
		}

		bool Archive::PushBlock(const Tag& tag)
		{
			//Create block
			Block block;

			if(m_blockStack.size() > 0)
			{
				//Set parent
				block.parent = &m_blockStack.back();
			}

			if(m_direction == eIn)
			{
				//Record block position
				block.startPos = m_stream.GetPosition();

				//Read header at current position
				Serialise(block.header);

				if(block.header.tag != tag)
				{
					//No match, record search start pos
					u64 searchStartPos = block.startPos;

					do
					{
						//If end of parent or end of file
						if(block.header.size == 0
							|| (block.parent && (block.startPos + block.header.size) >= (block.parent->startPos + block.parent->header.size))
							|| ((block.startPos + block.header.size) >= m_stream.GetSize()))
						{
							if(block.parent)
							{
								//End of parent, seek back to first child
								m_stream.Seek(block.parent->startPos + sizeof(Block::Header), eSeekModeStart);
							}
							else
							{
								//End of file, seek back to start
								m_stream.Seek(0, eSeekModeStart);
							}
						}
						else
						{
							//Seek to start of next block
							m_stream.Seek(block.startPos + block.header.size, eSeekModeStart);
						}

						//Record next block position
						block.startPos = m_stream.GetPosition();

						//Serialise next block header
						Serialise(block.header);

					} while(block.header.tag != tag && block.startPos != searchStartPos);

					if(block.header.tag != tag)
					{
						//Block not found, return to original starting position
						m_stream.Seek(searchStartPos, eSeekModeStart);
					}
				}
			}
			else
			{
				block.startPos = m_stream.GetPosition();
				block.header.tag = tag;
			}

			if(block.header.tag == tag)
			{
				//Block found (or being written), push to stack
				m_blockStack.push_back(block);

				//Success
				return true;
			}
			else
			{
				//Could not find block
				return false;
			}
		}

		void Archive::PopBlock()
		{
			debug::Assert(m_blockStack.size() > 0, "Archive::PopBlock() - No block to pop");

			//Get top block
			Block block = m_blockStack.back();

			//Pop block
			m_blockStack.pop_back();

			if(m_direction == eIn)
			{
				//Seek to block end
				m_stream.Seek(block.startPos + block.header.size, eSeekModeStart);
			}
			else
			{
				//Set block size
				block.header.size = sizeof(Block::Header) + block.data.GetSize();

				//Write current block
				Serialise(block.header);
				Serialise(block.data);
			}
		}

		void Archive::Block::Header::Serialise(Archive& archive)
		{
			archive.Serialise(tag.m_hash);
			archive.Serialise(size);
		}

		Archive::Tag::Tag()
		{
			m_hash = 0;
		}

		Archive::Tag::Tag(const char* string)
		{
			m_hash = Hash(string);
		}

		Archive::Tag::Tag(u32 hash)
		{
			m_hash = hash;
		}

		Archive::Tag::Tag(const Tag& rhs)
		{
			m_hash = rhs.m_hash;
		}

		Archive::Tag& Archive::Tag::operator = (u32 hash)
		{
			m_hash = hash;
			return *this;
		}

		bool Archive::Tag::operator == (const Tag& rhs) const
		{
			return m_hash == rhs.m_hash;
		}

		bool Archive::Tag::operator != (const Tag& rhs) const
		{
			return m_hash != rhs.m_hash;
		}

		void Archive::Serialise(MemoryStream& stream)
		{
			const u64 bufferSize = 1024;
			u8 buffer[bufferSize] = { 0 };

			if(GetDirection() == eIn)
			{
				debug::Error("Archive::Serialise() - Cannot serialise a stream back in");
			}
			else
			{
				u64 size = stream.GetSize();

				if(size > 0)
				{
					stream.Seek(0);

					while(size)
					{
						u64 bytesToWrite = maths::Min(size, bufferSize);
						stream.Read(buffer, bytesToWrite);
						Serialise((void*)buffer, bytesToWrite);
						size -= bytesToWrite;
					}
				}
			}
		}

		void Archive::Serialise(u8& data)
		{
			Serialise((void*)&data, sizeof(u8));
		}

		void Archive::Serialise(s8& data)
		{
			Serialise((void*)&data, sizeof(s8));
		}

		void Archive::Serialise(u16& data)
		{
			Serialise((void*)&data, sizeof(u16));
		}

		void Archive::Serialise(s16& data)
		{
			Serialise((void*)&data, sizeof(s16));
		}

		void Archive::Serialise(u32& data)
		{
			Serialise((void*)&data, sizeof(u32));
		}

		void Archive::Serialise(s32& data)
		{
			Serialise((void*)&data, sizeof(s32));
		}

		void Archive::Serialise(u64& data)
		{
			Serialise((void*)&data, sizeof(u64));
		}

		void Archive::Serialise(s64& data)
		{
			Serialise((void*)&data, sizeof(s64));
		}

		void Archive::Serialise(float& data)
		{
			Serialise((void*)&data, sizeof(float));
		}

		void Archive::Serialise(bool& data)
		{
			if(GetDirection() == eIn)
			{
				u8 boolean = 0;
				Serialise(boolean);
				data = (boolean != 0);
			}
			else
			{
				u8 boolean = data ? 1 : 0;
				Serialise(boolean);
			}
		}

		void Archive::Serialise(std::vector<u8>& data)
		{
			SerialisePODVector(data);
		}

		void Archive::Serialise(std::vector<s8>& data)
		{
			SerialisePODVector(data);
		}

		void Archive::Serialise(std::vector<u16>& data)
		{
			SerialisePODVector(data);
		}

		void Archive::Serialise(std::vector<s16>& data)
		{
			SerialisePODVector(data);
		}

		void Archive::Serialise(std::vector<u32>& data)
		{
			SerialisePODVector(data);
		}

		void Archive::Serialise(std::vector<s32>& data)
		{
			SerialisePODVector(data);
		}

		void Archive::Serialise(std::vector<u64>& data)
		{
			SerialisePODVector(data);
		}

		void Archive::Serialise(std::vector<s64>& data)
		{
			SerialisePODVector(data);
		}

		void Archive::Serialise(std::vector<float>& data)
		{
			SerialisePODVector(data);
		}

		void Archive::Serialise(std::string& string)
		{
			if(GetDirection() == eIn)
			{
				if(PushBlock("__string"))
				{
					//Serialise in num chars
					int numChars = 0;
					Serialise(numChars, "count");

					//Clear and reserve string
					string = "";
					string.reserve(numChars);

					//Serialise chars
					for(int i = 0; i < numChars; i++)
					{
						char character = 0;
						Serialise(character);
						string += character;
					}

					PopBlock();
				}
			}
			else
			{
				if(PushBlock("__string"))
				{
					//Serialise out num chars
					int numChars = (int)string.size();
					Serialise(numChars, "count");

					//Serialise out chars
					Serialise((void*)string.data(), numChars);

					PopBlock();
				}
			}
		}
	}
}