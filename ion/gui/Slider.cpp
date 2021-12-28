///////////////////////////////////////////////////
// File:		Slider.cpp
// Date:		8th November 2011
// Authors:		Matt Phillips
// Description:	UI horizontal slider
///////////////////////////////////////////////////

#include "Slider.h"

#include <ion/dependencies/imgui/imgui.h>

namespace ion
{
	namespace gui
	{
		Slider::Slider(const std::string& text, float min, float max, float defaultValue, float step, std::function<void(const Slider&, float)> const& onChanged)
			: m_onChanged(onChanged)
			, m_text(text)
			, m_value(defaultValue)
			, m_min(min)
			, m_max(max)
			, m_step(step)
		{

		}

		//Slider::Slider(const std::string& text, float min, int max, int defaultValue, int step)
		//	: m_text(text)
		//	, m_min(min)
		//	, m_max(max)
		//	, m_step(step)
		//	, m_value(defaultValue)
		//{

		//}

		Slider::~Slider()
		{
		}

		void Slider::SetRange(float min, float max, float step)
		{
			m_min = min;
			m_max = max;
			m_step = step;
		}

		void Slider::SetValue(float value)
		{
			m_value = value;
		}

		float Slider::GetValue() const
		{
			return m_value;
		}

		void Slider::Update(float deltaTime)
		{
			if (m_visible)
			{
				if (m_arrangement == Arrangement::Horizontal)
				{
					ImGui::SameLine();
				}

				float prevValue = m_value;

				ImGui::SliderFloat(m_text.c_str(), &m_value, m_min, m_max, "%.2f", 1.0f);

				if (prevValue != m_value)
				{
					m_onChanged(*this, m_value);
				}
			}
		}
	}
}