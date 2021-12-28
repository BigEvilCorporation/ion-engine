///////////////////////////////////////////////////
// File:		User.h
// Date:		11th October 2019
// Authors:		Matt Phillips
// Description:	Platform user interface
///////////////////////////////////////////////////

#pragma once

#include <string>

namespace ion
{
	namespace services
	{
		class User
		{
		public:
			virtual ~User() {}

			const std::string& Getname() { return m_name; }

		protected:
			User(const std::string& name) : m_name(name) {}

		private:
			std::string m_name;
		};
	}
}