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
			m_position.x = -1;
			m_position.y = -1;
			m_alpha = 1.0f;
		}

		Widget::Widget(const Vector2i& Position, const Vector2i& size)
			: m_position(Position)
			, m_size(size)
		{
			m_id = ion::GenerateUUID64();
			m_visible = true;
			m_centred = (Position.x != 0 || Position.y != 0) ? false : true;
			m_arrangement = Arrangement::Vertical;
			m_enabled = true;
			m_alpha = 1.0f;
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

		void Widget::SetPosition(const Vector2i& Position)
		{
			m_position = Position;
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

		void Widget::SetAlpha(float alpha)
		{
			m_alpha = alpha;
		}

		void Widget::SetEnabled(bool enabled)
		{
			m_enabled = enabled;
		}

		const Vector2i& Widget::GetSize() const
		{
			return m_size;
		}

		const Vector2i& Widget::GetPosition() const
		{
			return m_position;
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