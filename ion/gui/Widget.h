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

			enum class Arrangement
			{
				Vertical,
				Horizontal
			};

			Widget();
			Widget(const Vector2i& Position, const Vector2i& size);
			virtual ~Widget();

			virtual bool operator == (const Widget& rhs) const;

			UUID64 GetId() const;

			void SetPosition(const Vector2i& Position);
			void SetSize(const Vector2i& size);
			void SetCentred(bool centred);
			void SetArrangement(Arrangement arrangement);
			void SetAlpha(float alpha);
			void SetEnabled(bool enabled);

			const Vector2i& GetSize() const;
			const Vector2i& GetPosition() const;

			virtual void Show();
			virtual void Hide();

			virtual void Update(float deltaTime) = 0;

		protected:
			UUID64 m_id;
			bool m_visible;
			bool m_enabled;
			bool m_centred;
			Vector2i m_position;
			Vector2i m_size;
			Arrangement m_arrangement;
			float m_alpha;
		};
	}
}