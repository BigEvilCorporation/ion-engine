
////////////////////////////////////////////////////////////////
// Template definitions
////////////////////////////////////////////////////////////////

#include "core/io/Archive.h"
#include "resource/ResourceManager.h"
#include "core/io/File.h"

namespace ion
{
	namespace io
	{
		template <class T> bool ResourceT<T>::Load()
		{
			//Prepend directory
			std::string fullPath = m_resourceManager->GetResourceDirectory<T>();
			std::string extension = m_resourceManager->GetResourceExtension<T>();

			fullPath += "/";
			fullPath += m_filename;
			fullPath += extension;

			//Create and open scene file stream for reading
			File file(fullPath, File::OpenMode::Read);

			if (file.IsOpen())
			{
				//Create archive for serialising in
				Archive archiveIn(file, Archive::Direction::In, m_resourceManager);

				//Register pointer type
				T::RegisterSerialiseType(archiveIn);

				//Construct and serialise object
				m_resourceObject = archiveIn.ConstructAndSerialiseObject<T>();

				//Close file
				file.Close();

				//Loaded
				m_isLoaded = true;
			}
			else
			{
				ion::debug::log << "ResourceT<T>::Load() - Failed to open file " << fullPath << ion::debug::end;
			}

			return m_isLoaded;
		}

		template <class T> void ResourceT<T>::Unload()
		{
			m_isLoaded = false;
			delete m_resourceObject;
			m_resourceObject = NULL;
		}
	}
}