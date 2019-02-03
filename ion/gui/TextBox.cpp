///////////////////////////////////////////////////
// File:		TextBox.cpp
// Date:		7th November 2011
// Authors:		Matt Phillips
// Description:	UI editable text box
///////////////////////////////////////////////////

#include "TextBox.h"

#include <ion/dependencies/imgui/imgui.h>

namespace ion
{
	namespace gui
	{
		TextBox::TextBox()
			: m_text("")
		{

		}

		TextBox::TextBox(const std::string& text)
			: m_text(text)
		{
			
		}

		TextBox::~TextBox()
		{
		}

		void TextBox::SetText(const std::string& text)
		{
			m_text = text;
		}

		void TextBox::Update(float deltaTime)
		{
			if (m_visible)
			{
				if (m_arrangement == Arrangement::Horizontal)
				{
					ImGui::SameLine();
				}

				ImGui::Text("%s", m_text.c_str());
			}
		}
	}
}