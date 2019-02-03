///////////////////////////////////////////////////
// File:		DialogBox.h
// Date:		26th July 2018
// Authors:		Matt Phillips
// Description:	Dialog box
///////////////////////////////////////////////////

#pragma once

#include "Window.h"

#include <string>
#include <functional>

namespace ion
{
	namespace gui
	{
		class DialogBox : public Window
		{
		public:
			DialogBox(const std::string& title, std::function<void(const DialogBox&)> const& onClosed);

			void Close();

			virtual void Update(float deltaTime);

		private:
			std::function<void(const DialogBox&)> m_onClosed;
			bool m_opened;
			bool m_closed;
		};
	}
}