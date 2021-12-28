///////////////////////////////////////////////////
// File:		UserManager.h
// Date:		11th October 2019
// Authors:		Matt Phillips
// Description:	Platform user management interface
///////////////////////////////////////////////////

#pragma once

#include <functional>

namespace ion
{
	namespace services
	{
		class User;

		class UserManager
		{
		public:
			static UserManager* Create();

			enum class LoginResult
			{
				Success,
				Failed,
				Cancelled
			};

			typedef std::function<void(LoginResult result, User* user)> OnLoggedIn;
			typedef std::function<void(User& user)> OnLoggedOut;

			UserManager() {}
			virtual ~UserManager() {}

			virtual void Update() {}

			virtual void RequestLogin(OnLoggedIn const& onLoggedIn) = 0;
			virtual void RegisterCallbackLogout(OnLoggedOut const& loggedOut) = 0;
		};
	}
}