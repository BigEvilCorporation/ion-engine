///////////////////////////////////////////////////
	// File:		DialogBox.cpp
// Date:		26th July 2018
// Authors:		Matt Phillips
// Description:	Dialog box
///////////////////////////////////////////////////

#include "DialogBox.h"

#include <ion/dependencies/imgui/imgui.h>

namespace ion
{
	namespace gui
	{
		DialogBox::DialogBox(const std::string& title, std::function<void(const DialogBox&)> const& onClosed)
			: Window(title, Vector2i(), Vector2i())
			, m_onClosed(onClosed)
		{
			m_opened = false;
			m_closed = false;
		}

		void DialogBox::Close()
		{
			m_closed = true;
		}

		void DialogBox::Update(float deltaTime)
		{
			//Apply font
			if (m_font)
			{
				ImGui::PushFont(m_font->m_imFont);
			}

			if (!m_opened)
			{
				ImGui::OpenPopup(m_title.c_str());
				m_opened = true;
			}

			//Begin dialog
			bool open = true;
			if (ImGui::BeginPopupModal(m_title.c_str(), &open))
			{
				//Update widgets
				for (int i = 0; i < m_widgets.size(); i++)
				{
					m_widgets[i]->Update(deltaTime);
				}

				ImGui::EndPopup();
			}

			if (m_font)
			{
				ImGui::PopFont();
			}

			if (!open)
			{
				Close();
			}

			//Only fire closed callback after processing widgets and ending popup, since subscribers may delete the dialog
			if (m_closed && m_onClosed)
			{
				m_onClosed(*this);
			}
		}
	}
}