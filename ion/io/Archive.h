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

#include "io/Stream.h"

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

			//Default named block serialise
			template <typename T> void Serialise(T& object, const Tag& tag);

			//Register pointer type mapping
			template <typename T> void RegisterPointerType();

			//Register pointer type mapping with strictly defined serialise/runtime types
			template <typename SERIALISE_T, typename RUNTIME_T> void RegisterPointerTypeStrict();

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
				PointerMapping()
				{
					m_serialiseTypeName = typeid(SERIALISE_T).name();
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
			std::map<std::string, PointerMappingBase*> m_pointerMappingsBySerialiseName;
		};
	}
}

////////////////////////////////////////////////////////////////
// Template definitions
////////////////////////////////////////////////////////////////

#include "io/ResourceHandle.h"
#include "io/ResourceManager.h"

namespace ion
{
	namespace io
	{
		template <typename T> void Archive::RegisterPointerType()
		{
			std::string className = typeid(T).name();

			if(m_pointerMappingsBySerialiseName.find(className) == m_pointerMappingsBySerialiseName.end())
			{
				PointerMappingBase* pointerMapping = new PointerMapping<T, T>();
				m_pointerMappingsByRuntimeName[className] = pointerMapping;
				m_pointerMappingsBySerialiseName[className] = pointerMapping;
			}
		}

		template <typename SERIALISE_T, typename RUNTIME_T> void Archive:: RegisterPointerTypeStrict()
		{
			std::string serialiseName = typeid(SERIALISE_T).name();
			std::string runtimeName = typeid(RUNTIME_T).name();

			if(m_pointerMappingsByRuntimeName.find(runtimeName) == m_pointerMappingsByRuntimeName.end())
			{
				PointerMappingBase* pointerMapping = new PointerMapping<SERIALISE_T, RUNTIME_T>();
				m_pointerMappingsByRuntimeName[runtimeName] = pointerMapping;
				m_pointerMappingsBySerialiseName[serialiseName] = pointerMapping;
			}
		}

		template <typename T> void Archive::Serialise(T& object)
		{
			object.Serialise(*this);
		}

		template <typename T> void Archive::Serialise(T& object, const Tag& tag)
		{
			if(PushBlock(tag))
			{
				Serialise(object);
				PopBlock();
			}
		}

		template <typename T> void Archive::Serialise(T*& object)
		{
			//Serialise NULL flag
			bool nullPtr = (object == NULL);
			Serialise(nullPtr, "nullPtr");

			if(GetDirection() == Direction::In)
			{
				if(nullPtr)
				{
					//Pointer was NULL when serialised out
					object = NULL;
				}
				else
				{
					//Serialise class name
					std::string serialiseTypeName;
					Serialise(serialiseTypeName, "typeName");

					//Find in pointer mapping
					std::map<std::string, PointerMappingBase*>::iterator it = m_pointerMappingsBySerialiseName.find(serialiseTypeName);
					ion::debug::Assert(it != m_pointerMappingsBySerialiseName.end(), "Archive::Serialise(T*) - Pointer type not registered");

					//Construct
					object = (T*)it->second->Construct();

					//Serialise in
					it->second->Serialise(*this, object);
				}
			}
			else
			{
				if(!nullPtr)
				{
					//Serialising pointer out, determine actual type
					std::string runtimeClassName = typeid(*object).name();

					//Find in pointer mapping
					std::map<std::string, PointerMappingBase*>::iterator it = m_pointerMappingsByRuntimeName.find(runtimeClassName);
					ion::debug::Assert(it != m_pointerMappingsByRuntimeName.end(), "Archive::Serialise(T*) - Pointer type not registered");

					//Serialise class name
					Serialise(it->second->m_serialiseTypeName, "typeName");

					//Serialise out
					it->second->Serialise(*this, object);
				}
			}
		}

		template <typename T> void Archive::Serialise(ResourceHandle<T>& handle)
		{
			//Serialise NULL ptr flag
			bool validResource = handle.IsValid();
			Serialise(validResource, "valid");

			if(validResource)
			{
				if(GetDirection() == Archive::Direction::In)
				{
					debug::Assert(m_resourceManager != NULL, "No ResourceManager, cannot serialise in Resources with this archive");

					//Read resource name
					std::string resourceName;
					Serialise(resourceName, "name");

					//Request resource
					handle = m_resourceManager->GetResource<T>(resourceName);
				}
				else
				{
					//Write resource name
					std::string resourceName = handle.GetName();
					Serialise(resourceName, "name");
				}
			}
		}

		template <typename T1, typename T2> void Archive::Serialise(std::pair<T1, T2>& pair)
		{
			Serialise(pair.first, "first");
			Serialise(pair.second, "second");
		}

		//template <typename T1, typename T2, typename T3> void Archive::Serialise(std::tuple<T1, T2, T3>& tuple)
		//{
		//	Serialise(std::get<0>(tuple), "t1");
		//	Serialise(std::get<1>(tuple), "t2");
		//	Serialise(std::get<2>(tuple), "t3");
		//}

		template <typename T, int SIZE> void Archive::Serialise(FixedArray<T, SIZE>& objects)
		{
			if (GetDirection() == Direction::In)
			{
				//Serialise size
				int numObjects = 0;
				Serialise(numObjects, "count");

				//Sanity check
				debug::Assert(numObjects == SIZE, "Archive::Serialise(FixedArray<T, SIZE>&) - Array size changed");

				//Serialise all objects
				for (int i = 0; i < numObjects; i++)
				{
					Serialise(objects[i], "object");
				}
			}
			else
			{
				//Serialise out num objects
				int numObjects = SIZE;
				Serialise(numObjects, "count");

				//Serialise all objects out
				for (int i = 0; i < numObjects; i++)
				{
					Serialise(objects[i], "object");
				}
			}
		}

		template <typename T> void Archive::Serialise(std::vector<T>& objects)
		{
			if(GetDirection() == Direction::In)
			{
				//Serialise in num objects
				int numObjects = 0;
				Serialise(numObjects, "count");

				//Clear and reserve vector
				objects.clear();
				objects.resize(numObjects);

				//Serialise all objects
				for(int i = 0; i < numObjects; i++)
				{
					Serialise(objects[i], "object");
				}
			}
			else
			{
				//Serialise out num objects
				int numObjects = objects.size();
				Serialise(numObjects, "count");

				//Serialise all objects out
				for(int i = 0; i < numObjects; i++)
				{
					Serialise(objects[i], "object");
				}
			}
		}

		template <typename T> void Archive::SerialisePODVector(std::vector<T>& data)
		{
			if(GetDirection() == Direction::In)
			{
				//Serialise in num objects
				int numObjects = 0;
				Serialise(numObjects, "count");

				//Clear and reserve vector
				data.clear();

				//Serialise data
				if (numObjects > 0)
				{
					data.resize(numObjects);
					Serialise(&data[0], sizeof(T) * numObjects);
				}
			}
			else
			{
				//Serialise out num objects
				int numObjects = data.size();
				Serialise(numObjects, "count");

				//Serialise data
				if (numObjects > 0)
				{
					Serialise(&data[0], sizeof(T) * numObjects);
				}
			}
		}

		template <typename T> void Archive::Serialise(std::list<T>& objects)
		{
			if(GetDirection() == Direction::In)
			{
				//Serialise in num objects
				int numObjects = 0;
				Serialise(numObjects, "count");

				//Clear list
				objects.clear();

				//Serialise all objects and add to list
				for(int i = 0; i < numObjects; i++)
				{
					T object;
					Serialise(object, "object");
					objects.push_back(object);
				}
			}
			else
			{
				//Serialise out num objects
				int numObjects = objects.size();
				Serialise(numObjects, "count");

				//Serialise all objects out
				for(typename std::list<T>::iterator it = objects.begin(), end = objects.end(); it != end; ++it)
				{
					Serialise(*it, "object");
				}
			}
		}

		template <typename KEY, typename T> void Archive::Serialise(std::map<KEY, T>& objects)
		{
			if(GetDirection() == Direction::In)
			{
				//Serialise in num objects
				int numObjects = 0;
				Serialise(numObjects);

				//Clear map
				objects.clear();

				//Serialise all objects and add to map
				for(int i = 0; i < numObjects; i++)
				{
					//Serialise key
					KEY key;
					Serialise(key, "key");

					//Add to map
					auto it = objects.insert(std::make_pair(key, T()));

					//Serialise object
					Serialise(it.first->second, "object");
				}
			}
			else
			{
				//Serialise out num objects
				int numObjects = (int)objects.size();
				Serialise(numObjects);

				//Serialise all objects out
				for(typename std::map<KEY, T>::iterator it = objects.begin(), end = objects.end(); it != end; ++it)
				{
					//Serialise key (lose const correctness, direction is known)
					Serialise((KEY&)it->first, "key");

					//Serialise object
					Serialise(it->second, "object");
				}
			}
		}
	}
}

#endif // Header guard
