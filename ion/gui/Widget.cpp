///////////////////////////////////////////////////
// File:		Widget.cpp
// Date:		22nd September 2011
// Authors:		Matt Phillips
// Description:	UI widget base class
///////////////////////////////////////////////////

#include "Widget.h"

namespace ion
{
	namespace gui
	{
		Widget::Widget()
		{
			m_visible = true;
			m_id = ion::GenerateUUID64();
		}

		Widget::~Widget()
		{
		}

		void Widget::AddChild(Widget& widget)
		{
		}

		void Widget::SetPosition(const Vector2& position)
		{
		}

		void Widget::SetSize(const Vector2& size)
		{
		}

		void Widget::Show()
		{
			m_visible = true;
		}

		void Widget::Hide()
		{
			m_visible = false;
		}
	}
}