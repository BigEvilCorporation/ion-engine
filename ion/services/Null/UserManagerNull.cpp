///////////////////////////////////////////////////
// File:		UserManagerNull.cpp
// Date:		11th October 2019
// Authors:		Matt Phillips
// Description:	Null user management interface
///////////////////////////////////////////////////

#include "UserManagerNull.h"
#include "UserNull.h"

namespace ion
{
	namespace services
	{
		UserManager* UserManager::Create()
		{
			return new UserManagerNull();
		}

		UserManagerNull::UserManagerNull()
		{

		}

		UserManagerNull::~UserManagerNull()
		{

		}

		void UserManagerNull::Update()
		{

		}

		void UserManagerNull::RequestLogin(OnLoggedIn const& onLoggedIn)
		{
			UserNull* user = new UserNull("user");
			m_users.push_back(user);
			onLoggedIn(LoginResult::Success, user);
		}

		void UserManagerNull::RegisterCallbackLogout(OnLoggedOut const& onLoggedOut)
		{

		}
	}
}