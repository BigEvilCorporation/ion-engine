///////////////////////////////////////////////////
// File:		Button.h
// Date:		6th November 2011
// Authors:		Matt Phillips
// Description:	UI button
///////////////////////////////////////////////////

#pragma once

#include "Widget.h"

#include <string>
#include <functional>

namespace ion
{
	namespace gui
	{
		class Button : public Widget
		{
		public:

			enum class TextAlign
			{
				Left,
				Right,
				Centre
			};

			Button(const std::string& text, std::function<void(const Button&)> const& onPressed);
			virtual ~Button();

			void SetTextAlign(TextAlign align);

			virtual void Update(float deltaTime);

		private:
			std::string m_text;
			TextAlign m_textAlign;
			std::function<void(const Button&)> m_onPressed;
		};
	}
}