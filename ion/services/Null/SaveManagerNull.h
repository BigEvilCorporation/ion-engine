///////////////////////////////////////////////////
// File:		SaveManagerNull.h
// Date:		11th October 2019
// Authors:		Matt Phillips
// Description:	Null save game interface
///////////////////////////////////////////////////

#pragma once

#include <services/SaveManager.h>

namespace ion
{
	namespace services
	{
		class SaveManagerNull : public SaveManager
		{
		public:
			SaveManagerNull();
			virtual ~SaveManagerNull();

			virtual void InitialiseSaveStorage(const User& user, const std::string& saveName, OnSaveInitialised const& onSaveInitialised);
			virtual void RequestSave(const User& user, const std::string& saveName, const ion::io::MemoryStream& data, OnSaveCompleted const& onSaveCompleted);
			virtual void RequestLoad(const User& user, const std::string& saveName, OnLoadCompleted const& onLoadCompleted);
		};
	}
}