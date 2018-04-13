///////////////////////////////////////////////////
// File:		Button.cpp
// Date:		6th November 2011
// Authors:		Matt Phillips
// Description:	UI button
///////////////////////////////////////////////////

#include "Button.h"

#include <ion/dependencies/imgui/imgui.h>

namespace ion
{
	namespace gui
	{
		Button::Button(const std::string& text, std::function<void(const Button&)> const& onPressed)
			: m_onPressed(onPressed)
			, m_text(text)
		{
		}

		Button::~Button()
		{
		}

		void Button::Update(float deltaTime)
		{
			if (ImGui::Button(m_text.c_str()))
			{
				m_onPressed(*this);
			}
		}
	}
}