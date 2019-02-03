///////////////////////////////////////////////////
// File:		CheckBox.h
// Date:		30th June 2018
// Authors:		Matt Phillips
// Description:	UI check box
///////////////////////////////////////////////////

#include "CheckBox.h"

#include <ion/dependencies/imgui/imgui.h>

namespace ion
{
	namespace gui
	{
		CheckBox::CheckBox(const std::string& text, std::function<void(const CheckBox&, bool)> const& onSelected)
			: m_text(text),
			m_onSelected(onSelected)
		{
			m_checked = false;
		}

		void CheckBox::SetChecked(bool checked)
		{
			m_checked = checked;
		}

		bool CheckBox::GetChecked() const
		{
			return m_checked;
		}

		void CheckBox::Update(float deltaTime)
		{
			if (m_visible)
			{
				bool prevChecked = m_checked;

				if (m_arrangement == Arrangement::Horizontal)
				{
					ImGui::SameLine();
				}

				ImGui::Checkbox(m_text.c_str(), &m_checked);

				if (m_checked != prevChecked)
				{
					m_onSelected(*this, m_checked);
				}
			}
		}
	}
}