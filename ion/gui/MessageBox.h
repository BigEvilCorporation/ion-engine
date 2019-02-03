///////////////////////////////////////////////////
// File:		MessageBox.h
// Date:		26th July 2018
// Authors:		Matt Phillips
// Description:	Message box
///////////////////////////////////////////////////

#pragma once

#include "DialogBox.h"
#include "Button.h"
#include "TextBox.h"

#include <string>
#include <functional>
#include <vector>

namespace ion
{
	namespace gui
	{
		class MessageBox : public DialogBox
		{
		public:
			enum ButtonType
			{
				Cancel	= (1 << 0),
				Ok		= (1 << 1),
				No		= (1 << 2),
				Yes		= (1 << 3),
				Abort	= (1 << 4),
				Retry	= (1 << 5),
				Ignore	= (1 << 6),
			};

			MessageBox(const std::string& title, const std::string& text, int buttonTypeMask, std::function<void(const MessageBox&, ButtonType buttonPressed)> const& onDismissed);
			virtual ~MessageBox();

		private:
			static const int s_maxButtons = 7;
			static const char* s_buttonLabels[s_maxButtons];

			void OnButtonPressed(const ion::gui::Button& button);
			void OnDlgClosed(const ion::gui::DialogBox& dialog);

			std::function<void(const MessageBox&, ButtonType buttonPressed)> m_onDismissed;

			TextBox m_textBox;
			std::vector<Button*> m_buttons;
		};
	}
}