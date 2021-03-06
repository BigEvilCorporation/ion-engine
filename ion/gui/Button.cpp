///////////////////////////////////////////////////
// File:		Button.cpp
// Date:		6th November 2011
// Authors:		Matt Phillips
// Description:	UI button
///////////////////////////////////////////////////

#include "Button.h"

#include <ion/dependencies/imgui/imgui.h>
#include <ion/dependencies/imgui/imgui_internal.h>

namespace ion
{
	namespace gui
	{
		Button::Button(const std::string& text, std::function<void(const Button&)> const& onPressed)
			: m_onPressed(onPressed)
			, m_text(text)
		{
			m_textAlign = TextAlign::Centre;
		}

		Button::~Button()
		{
		}

		void Button::SetTextAlign(TextAlign align)
		{
			m_textAlign = align;
		}

		void Button::Update(float deltaTime)
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
				else if(m_position.x != 0 && m_position.y != 0)
				{
					ImGui::SetCursorPos(ImVec2(m_position.x, m_position.y));
				}

				if (m_arrangement == Arrangement::Horizontal)
				{
					ImGui::SameLine();
				}

				switch (m_textAlign)
				{
				case TextAlign::Left:
					ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.0f, 0.5f));
					break;
				case TextAlign::Right:
					ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(1.0f, 0.5f));
					break;
				case TextAlign::Centre:
					ImGui::PushStyleVar(ImGuiStyleVar_ButtonTextAlign, ImVec2(0.5f, 0.5f));
					break;
				}

				if (!m_enabled)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				if (ImGui::Button(m_text.c_str(), ImVec2(m_size.x, m_size.y)))
				{
					m_onPressed(*this);
				}

				if (!m_enabled)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::PopStyleVar();
			}
		}
	}
}