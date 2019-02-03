///////////////////////////////////////////////////
// File:		CheckBox.h
// Date:		30th June 2018
// Authors:		Matt Phillips
// Description:	UI check box
///////////////////////////////////////////////////

#pragma once

#include "Widget.h"

#include <string>
#include <functional>

namespace ion
{
	namespace gui
	{
		class CheckBox : public Widget
		{
		public:
			CheckBox(const std::string& text, std::function<void(const CheckBox&, bool)> const& onSelected);

			void SetChecked(bool checked);
			bool GetChecked() const;

			virtual void Update(float deltaTime);

		private:
			std::string m_text;
			std::function<void(const CheckBox&, bool)> m_onSelected;
			bool m_checked;
		};
	}
}