///////////////////////////////////////////////////
// File:		SaveManagerNull.cpp
// Date:		11th October 2019
// Authors:		Matt Phillips
// Description:	Null save game interface
///////////////////////////////////////////////////

#include "SaveManagerNull.h"

namespace ion
{
	namespace services
	{
		SaveManager* SaveManager::Create()
		{
			return new SaveManagerNull();
		}

		SaveManagerNull::SaveManagerNull()
		{

		}

		SaveManagerNull::~SaveManagerNull()
		{

		}

		void SaveManagerNull::InitialiseSaveStorage(const User& user, const std::string& saveName, OnSaveInitialised const& onSaveInitialised)
		{

		}

		void SaveManagerNull::RequestSave(const User& user, const std::string& saveName, const ion::io::MemoryStream& data, OnSaveCompleted const& onSaveCompleted)
		{

		}

		void SaveManagerNull::RequestLoad(const User& user, const std::string& saveName, OnLoadCompleted const& onLoadCompleted)
		{
		}
	}
}