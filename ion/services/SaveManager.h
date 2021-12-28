///////////////////////////////////////////////////
// File:		SaveManager.h
// Date:		11th October 2019
// Authors:		Matt Phillips
// Description:	Platform save game interface
///////////////////////////////////////////////////

#pragma once

#include <ion/core/Types.h>
#include <ion/core/io/Stream.h>

#include <functional>
#include <vector>
#include <string>

namespace ion
{
	namespace services
	{
		class User;

		class SaveManager
		{
		public:
			enum class InitResult
			{
				Success,
				Failed,
				OutOfSpace
			};

			enum class SaveResult
			{
				Success,
				Failed
			};

			enum class LoadResult
			{
				Success,
				Failed
			};

			static SaveManager* Create();

			typedef std::function<void(InitResult result)> OnSaveInitialised;
			typedef std::function<void(SaveResult result)> OnSaveCompleted;
			typedef std::function<void(LoadResult result, ion::io::MemoryStream& data)> OnLoadCompleted;

			SaveManager() {}
			virtual ~SaveManager() {}

			virtual void InitialiseSaveStorage(const User& user, const std::string& saveName, OnSaveInitialised const& onSaveInitialised) = 0;
			virtual void RequestSave(const User& user, const std::string& saveName, const ion::io::MemoryStream& data, OnSaveCompleted const& onSaveCompleted) = 0;
			virtual void RequestLoad(const User& user, const std::string& saveName, OnLoadCompleted const& onLoadCompleted) = 0;
		};
	}
}