///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Archive.h
// Date:		22nd November 2012
// Authors:		Matt Phillips
// Description:	Serialisable archive
///////////////////////////////////////////////////

#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "core/io/Stream.h"

//Includes for ion types handled directly by Archive
#include "core/Types.h"
#include "core/containers/FixedArray.h"
#include "maths/Maths.h"

//Includes STL types handled directly by Archive
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>

//RTTI
#include <typeinfo>

namespace ion
{
	namespace io
	{
		class ResourceManager;
		template <class T> class ResourceHandle;

		class Archive
		{
		public:
			enum class Direction { In, Out };
			enum class Content { Full, Minimal };

			struct Tag
			{
				Tag();
				Tag(const char* string);
				Tag(u32 hash);
				Tag(const Tag& rhs);
				Tag& operator = (u32 hash);
				bool operator == (const Tag& rhs) const;
				bool operator != (const Tag& rhs) const;

				u32 m_hash;
			};

			Archive(Stream& stream, Direction direction, ResourceManager* resourceManager = NULL, u32 version = 0);

			void SetContentType(Content content);
			Content GetContentType() const;

			u64 GetStreamSize() const { return m_stream.GetSize(); }

			//Default named block serialise
			template <typename T> void Serialise(T& object, const Tag& tag);

			//Register pointer type mapping
			template <typename T> void RegisterPointerType(const std::string& serialiseName);

			//Register pointer type mapping with strictly defined serialise/runtime types
			template <typename SERIALISE_T, typename RUNTIME_T> void RegisterPointerTypeStrict(const std::string& serialiseName);

			//Create and serialise runtime object from base typename
			template <typename T> T* ConstructAndSerialiseObject();

			//Create and serialise runtime object from serialised typename
			template <typename T> T* ConstructAndSerialiseObject(const std::string serialiseName);

			Direction GetDirection() const;
			u32 GetVersion() const;
			ResourceManager* GetResourceManager() const;

		//private:

			//Named blocks
			bool PushBlock(const Tag& tag);
			void PopBlock();

			//Default templated serialise
			template <typename T> void Serialise(T& object);

			//Pointer serialise
			template <typename T> void Serialise(T*& object);

			//Resource serialise
			template <typename T> void Serialise(ResourceHandle<T>& object);

			//Stream serialise
			void Serialise(MemoryStream& stream);

			//Raw serialisation (no endian flipping)
			void Serialise(void* data, u64 size);

			//Raw serialisation of basic types (with endian flipping)
			void Serialise(u8& data);
			void Serialise(s8& data);
			void Serialise(u16& data);
			void Serialise(s16& data);
			void Serialise(u32& data);
			void Serialise(s32& data);
			void Serialise(u64& data);
			void Serialise(s64& data);
			void Serialise(float& data);
			void Serialise(bool& data);

			//Raw serialisation of vectors of basic types
			void Serialise(std::vector<u8>& data);
			void Serialise(std::vector<s8>& data);
			void Serialise(std::vector<u16>& data);
			void Serialise(std::vector<s16>& data);
			void Serialise(std::vector<u32>& data);
			void Serialise(std::vector<s32>& data);
			void Serialise(std::vector<u64>& data);
			void Serialise(std::vector<s64>& data);
			void Serialise(std::vector<float>& data);

			template <typename T> void SerialisePODVector(std::vector<T>& data);

			//Serialise ion containers
			template <typename T, int SIZE> void Serialise(FixedArray<T, SIZE>& objects);

			//Serialise STL string
			void Serialise(std::string& string);

			//Serialise STL containers
			template <typename T1, typename T2> void Serialise(std::pair<T1, T2>& pair);
			//template <typename T1, typename T2, typename T3> void Serialise(std::tuple<T1, T2, T3>& tuple);
			template <typename T> void Serialise(std::vector<T>& objects);
			template <typename T> void Serialise(std::list<T>& objects);
			template <typename KEY, typename T> void Serialise(std::map<KEY, T>& objects);

		private:

			//i/o stream
			Stream& m_stream;

			//Stream version
			u32 m_version;

			//Serialise direction
			Direction m_direction;

			//Content type
			Content m_contentType;

			//Resource manager
			ResourceManager* m_resourceManager;

			//Block
			struct Block
			{
				#pragma pack(push, 1)
				struct Header
				{
					Header()
					{
						tag = 0;
						size = 0;
					}

					Tag tag;
					u64 size;

					void Serialise(Archive& archive);
				};
				#pragma pack(pop)

				Block()
				{
					startPos = 0;
					parent = NULL;
				}

				Header header;
				MemoryStream data;
				u64 startPos;
				Block* parent;
			};

			//Block stack
			std::vector<Block> m_blockStack;

			//Pointer constructor/serialiser
			struct PointerMappingBase
			{
				virtual void* Construct() { return NULL; }
				virtual void Serialise(Archive& archive, void* pointer) {}

				std::string m_serialiseTypeName;
				std::string m_runtimeTypeName;
			};

			template <typename SERIALISE_T, typename RUNTIME_T> struct PointerMapping : public PointerMappingBase
			{
				PointerMapping(const std::string& serialiseName)
				{
					m_serialiseTypeName = serialiseName;
					m_runtimeTypeName = typeid(RUNTIME_T).name();
				}

				virtual void* Construct()
				{
					return (void*)new RUNTIME_T;
				}

				virtual void Serialise(Archive& archive, void* pointer)
				{
					archive.Serialise(*(SERIALISE_T*)(pointer));
				}
			};

			//Registered pointer mappings
			std::map<std::string, PointerMappingBase*> m_pointerMappingsByRuntimeName;
			std::map<std::string, PointerMappingBase*> m_pointerMappingsByBaseName;
			std::map<std::string, PointerMappingBase*> m_pointerMappingsBySerialiseName;
		};
	}
}

//Inline/template includes
#include "Archive.inl"

#endif // Header guard
