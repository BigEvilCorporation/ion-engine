///////////////////////////////////////////////////
// File:		UserManagerNull.h
// Date:		11th October 2019
// Authors:		Matt Phillips
// Description:	Null user management interface
///////////////////////////////////////////////////

#pragma once

#include <services/UserManager.h>

#include <vector>

namespace ion
{
	namespace services
	{
		class UserNull;

		class UserManagerNull : public UserManager
		{
		public:
			UserManagerNull();
			virtual ~UserManagerNull();

			virtual void Update();

			virtual void RequestLogin(OnLoggedIn const& onLoggedIn);
			virtual void RegisterCallbackLogout(OnLoggedOut const& onLoggedOut);

		private:
			std::vector<UserNull*> m_users;
		};
	}
}