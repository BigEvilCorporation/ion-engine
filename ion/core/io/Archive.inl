
////////////////////////////////////////////////////////////////
// Template definitions
////////////////////////////////////////////////////////////////
#include "resource/ResourceHandle.h"
#include "resource/ResourceManager.h"

namespace ion
{
	namespace io
	{
		template <typename BASE_T> void Archive::RegisterPointerType(const std::string& serialiseName)
		{
			std::string baseName = typeid(BASE_T).name();

			if (m_pointerMappingsBySerialiseName.find(serialiseName) == m_pointerMappingsBySerialiseName.end())
			{
				PointerMappingBase* pointerMapping = new PointerMapping<BASE_T, BASE_T>(serialiseName);
				m_pointerMappingsByRuntimeName[baseName] = pointerMapping;
				m_pointerMappingsByBaseName[baseName] = pointerMapping;
				m_pointerMappingsBySerialiseName[serialiseName] = pointerMapping;
			}
		}

		template <typename BASE_T, typename RUNTIME_T> void Archive::RegisterPointerTypeStrict(const std::string& serialiseName)
		{
			std::string baseName = typeid(BASE_T).name();
			std::string runtimeName = typeid(RUNTIME_T).name();

			if (m_pointerMappingsByRuntimeName.find(runtimeName) == m_pointerMappingsByRuntimeName.end())
			{
				PointerMappingBase* pointerMapping = new PointerMapping<BASE_T, RUNTIME_T>(serialiseName);
				m_pointerMappingsByRuntimeName[runtimeName] = pointerMapping;
				m_pointerMappingsByBaseName[baseName] = pointerMapping;
				m_pointerMappingsBySerialiseName[serialiseName] = pointerMapping;
			}
		}

		template <typename BASE_T> BASE_T* Archive::ConstructAndSerialiseObject()
		{
			std::string baseName = typeid(BASE_T).name();

			std::map<std::string, PointerMappingBase*>::iterator it = m_pointerMappingsByBaseName.find(baseName);
			if (it == m_pointerMappingsByBaseName.end())
			{
				ion::debug::log << "Archive::ConstructAndSerialiseObject<T>() - Object type \'" << baseName << "\' not registered" << ion::debug::end;
			}

			//Construct
			BASE_T* object = (BASE_T*)it->second->Construct();

			//Serialise in
			it->second->Serialise(*this, object);

			return object;
		}

		template <typename BASE_T> BASE_T* Archive::ConstructAndSerialiseObject(const std::string serialiseName)
		{
			std::map<std::string, PointerMappingBase*>::iterator it = m_pointerMappingsBySerialiseName.find(serialiseName);
			if (it == m_pointerMappingsBySerialiseName.end())
			{
				ion::debug::log << "Archive::ConstructAndSerialiseObject<T>(string) - Object type \'" << serialiseName << "\' not registered" << ion::debug::end;
			}

			//Construct
			BASE_T* object = (BASE_T*)it->second->Construct();

			//Serialise in
			it->second->Serialise(*this, object);

			return object;
		}

		template <typename T> void Archive::Serialise(T& object)
		{
			object.Serialise(*this);
		}

		template <typename T> void Archive::Serialise(T& object, const Tag& tag)
		{
			if (PushBlock(tag))
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

			if (GetDirection() == Direction::In)
			{
				if (nullPtr)
				{
					//Pointer was NULL when serialised out
					object = NULL;
				}
				else
				{
					//Serialise class name
					std::string serialiseTypeName;
					Serialise(serialiseTypeName, "typeName");

					//Construct and serialise in
					object = ConstructAndSerialiseObject<T>(serialiseTypeName);
				}
			}
			else
			{
				if (!nullPtr)
				{
					//Serialising pointer out, determine actual type
					std::string runtimeClassName = typeid(*object).name();

					//Find in pointer mapping
					std::map<std::string, PointerMappingBase*>::iterator it = m_pointerMappingsByRuntimeName.find(runtimeClassName);
					if (it == m_pointerMappingsByRuntimeName.end())
					{
						ion::debug::error << "Archive::Serialise(T*) - Pointer type \'" << runtimeClassName << "\' << not registered" << ion::debug::end;
					}

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

			if (validResource)
			{
				if (GetDirection() == Archive::Direction::In)
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
			if (GetDirection() == Direction::In)
			{
				//Serialise in num objects
				int numObjects = 0;
				Serialise(numObjects, "count");

				//Clear and reserve vector
				objects.clear();
				objects.resize(numObjects);

				//Serialise all objects
				for (int i = 0; i < numObjects; i++)
				{
					Serialise(objects[i], "object");
				}
			}
			else
			{
				//Serialise out num objects
				int numObjects = (int)objects.size();
				Serialise(numObjects, "count");

				//Serialise all objects out
				for (int i = 0; i < numObjects; i++)
				{
					Serialise(objects[i], "object");
				}
			}
		}

		template <typename T> void Archive::SerialisePODVector(std::vector<T>& data)
		{
			if (GetDirection() == Direction::In)
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
				int numObjects = (int)data.size();
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
			if (GetDirection() == Direction::In)
			{
				//Serialise in num objects
				int numObjects = 0;
				Serialise(numObjects, "count");

				//Clear list
				objects.clear();

				//Serialise all objects and add to list
				for (int i = 0; i < numObjects; i++)
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
				for (typename std::list<T>::iterator it = objects.begin(), end = objects.end(); it != end; ++it)
				{
					Serialise(*it, "object");
				}
			}
		}

		template <typename KEY, typename T> void Archive::Serialise(std::map<KEY, T>& objects)
		{
			if (GetDirection() == Direction::In)
			{
				//Serialise in num objects
				int numObjects = 0;
				Serialise(numObjects);

				//Clear map
				objects.clear();

				//Serialise all objects and add to map
				for (int i = 0; i < numObjects; i++)
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
				for (typename std::map<KEY, T>::iterator it = objects.begin(), end = objects.end(); it != end; ++it)
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