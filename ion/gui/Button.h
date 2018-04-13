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
			Button(const std::string& text, std::function<void(const Button&)> const& onPressed);
			virtual ~Button();

			virtual void Update(float deltaTime);

		private:
			std::string m_text;
			std::function<void(const Button&)> const& m_onPressed;
		};
	}
}