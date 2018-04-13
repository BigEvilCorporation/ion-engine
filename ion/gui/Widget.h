///////////////////////////////////////////////////
// File:		Widget.h
// Date:		22nd September 2011
// Authors:		Matt Phillips
// Description:	UI widget base class
///////////////////////////////////////////////////

#pragma once

#include <ion/maths/Vector.h>
#include <ion/core/cryptography/UUID.h>

#include <string>

namespace ion
{
	namespace gui
	{
		class Widget
		{
		public:

			Widget();
			virtual ~Widget();

			UUID64 GetId() const;

			void AddChild(Widget& widget);

			void SetPosition(const Vector2& position);
			void SetSize(const Vector2& size);

			virtual void Show();
			virtual void Hide();

			virtual void Update(float deltaTime) = 0;

		private:
			UUID64 m_id;
			bool m_visible;
		};
	}
}