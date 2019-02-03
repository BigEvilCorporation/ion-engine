///////////////////////////////////////////////////
// File:		MessageBox.cpp
// Date:		26th July 2018
// Authors:		Matt Phillips
// Description:	Message box
///////////////////////////////////////////////////

#include "MessageBox.h"

namespace ion
{
	namespace gui
	{
		const char* MessageBox::s_buttonLabels[s_maxButtons] =
		{
			"Cancel",
			"Ok",
			"No",
			"Yes",
			"Abort",
			"Retry",
			"Ignore",
		};

		MessageBox::MessageBox(const std::string& title, const std::string& text, int buttonTypeMask, std::function<void(const MessageBox&, ButtonType buttonPressed)> const& onDismissed)
			: DialogBox(title, nullptr)
			, m_onDismissed(onDismissed)
			, m_textBox(text)
		{
			m_buttons.resize(s_maxButtons);

			AddWidget(m_textBox);

			for (int i = 0; i < s_maxButtons; i++)
			{
				if ((1 << i) & buttonTypeMask)
				{
					m_buttons[i] = new Button(s_buttonLabels[i], std::bind(&MessageBox::OnButtonPressed, this, std::placeholders::_1));
					AddWidget(*m_buttons[i]);
				}
			}
		}

		MessageBox::~MessageBox()
		{
			for (int i = 0; i < s_maxButtons; i++)
			{
				if (m_buttons[i])
				{
					delete m_buttons[i];
				}
			}

			m_buttons.clear();
		}

		void MessageBox::OnButtonPressed(const ion::gui::Button& button)
		{
			if (m_onDismissed)
			{
				for (int i = 0; i < s_maxButtons; i++)
				{
					if (m_buttons[i] == &button)
					{
						m_onDismissed(*this, (ButtonType)(1 << i));
						return;
					}
				}
			}
		}
	}
}