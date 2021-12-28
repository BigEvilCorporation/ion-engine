///////////////////////////////////////////////////
// File:		TextBox.h
// Date:		7th November 2011
// Authors:		Matt Phillips
// Description:	UI editable text box
///////////////////////////////////////////////////

#pragma once

#include "Widget.h"
#include <string>

namespace ion
{
	namespace gui
	{
		class TextBox : public Widget
		{
		public:
			TextBox();
			TextBox(const std::string& text);
			virtual ~TextBox();

			void SetText(const std::string& text);
			void SetScroll(float scroll);

			virtual void Update(float deltaTime);

		private:
			std::string m_text;
			float m_scrollOverride;
			UUID64 m_id;
		};
	}
}