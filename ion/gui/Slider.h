///////////////////////////////////////////////////
// File:		Slider.h
// Date:		8th November 2011
// Authors:		Matt Phillips
// Description:	UI horizontal slider
///////////////////////////////////////////////////

#pragma once

#include "Widget.h"

#include <string>
#include <functional>

namespace ion
{
	namespace gui
	{
		class Slider : public Widget
		{
		public:
			enum class DataType { Integer, Float };

			Slider(const std::string& text, float min, float max, float defaultValue, float step, std::function<void(const Slider&, float)> const& onChanged);
			~Slider();

			void SetRange(float min, float max, float step);
			void SetValue(float value);
			float GetValue() const;

			virtual void Update(float deltaTime);

		private:
			std::function<void(const Slider&, float)> m_onChanged;
			std::string m_text;
			float m_value;
			float m_min;
			float m_max;
			float m_step;
		};
	}
}