///////////////////////////////////////////////////
// File:		TextBox.cpp
// Date:		7th November 2011
// Authors:		Matt Phillips
// Description:	UI editable text box
///////////////////////////////////////////////////

#include "TextBox.h"
#include <core/cryptography/UUID.h>
#include <ion/dependencies/imgui/imgui.h>

namespace ion
{
	namespace gui
	{
		TextBox::TextBox()
			: m_text("")
		{
			m_id = ion::GenerateUUID64();
			m_scrollOverride = -1;
		}

		TextBox::TextBox(const std::string& text)
			: m_text(text)
		{
			m_id = ion::GenerateUUID64();
			m_scrollOverride = -1;
		}

		TextBox::~TextBox()
		{
		}

		void TextBox::SetText(const std::string& text)
		{
			m_text = text;
		}

		void TextBox::SetScroll(float scroll)
		{
			m_scrollOverride = scroll;
		}

		void TextBox::Update(float deltaTime)
		{
			if (m_visible)
			{
				if (m_centred)
				{
					if (m_size.x == 0)
					{
						ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2) - (ImGui::CalcTextSize(m_text.c_str()).x / 2));
					}
					else
					{
						ImGui::SetCursorPosX((ImGui::GetWindowWidth() / 2) - (m_size.x / 2));
					}
				}
				else if (m_position.x != -1 && m_position.y != -1)
				{
					ImGui::SetCursorPos(ImVec2((float)m_position.x, (float)m_position.y));
				}

				if (m_arrangement == Arrangement::Horizontal)
				{
					ImGui::SameLine();
				}

				// Child window allows independent scroll bars
				ImGui::BeginChild((int)m_id & 0xFFFFFFFF, ImVec2(m_size.x, m_size.y));
				ImGui::PushTextWrapPos();

				if (m_scrollOverride >= 0.0f && m_scrollOverride <= 1.0f)
				{
					ImGui::SetScrollHere(m_scrollOverride);
					m_scrollOverride = -1;
				}

				ImGui::Text("%s", m_text.c_str());
				ImGui::PopTextWrapPos();
				ImGui::EndChild();
			}
		}
	}
}