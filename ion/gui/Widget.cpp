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
			m_id = ion::GenerateUUID64();
			m_visible = true;
			m_centred = true;
			m_arrangement = Arrangement::Vertical;
			m_enabled = true;
		}

		Widget::Widget(const Vector2i& position, const Vector2i& size)
			: m_position(position)
			, m_size(size)
		{
			m_id = ion::GenerateUUID64();
			m_visible = true;
			m_centred = (position.x != 0 || position.y != 0) ? false : true;
			m_arrangement = Arrangement::Vertical;
			m_enabled = true;
		}

		Widget::~Widget()
		{
		}

		bool Widget::operator == (const Widget& rhs) const
		{
			return m_id == rhs.GetId();
		}

		UUID64 Widget::GetId() const
		{
			return m_id;
		}

		void Widget::SetPosition(const Vector2i& position)
		{
			m_position = position;
		}

		void Widget::SetSize(const Vector2i& size)
		{
			m_size = size;
		}

		void Widget::SetCentred(bool centred)
		{
			m_centred = centred;
		}

		void Widget::SetArrangement(Arrangement arrangement)
		{
			m_arrangement = arrangement;
		}

		void Widget::SetEnabled(bool enabled)
		{
			m_enabled = enabled;
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