///////////////////////////////////////////////////
// File:		Window.h
// Date:		23nd September 2011
// Authors:		Matt Phillips
// Description:	UI window
///////////////////////////////////////////////////

#pragma once

#include "Widget.h"
#include "Font.h"

#include <ion/core/cryptography/UUID.h>

#include <string>

namespace ion
{
	namespace gui
	{
		class Window : public Widget
		{
		public:
			Window(const std::string& title, const Vector2i& position, const Vector2i& size);
			virtual ~Window();

			void AddWidget(Widget& widget);

			void AllowResize(bool allow);
			void AllowMove(bool allow);
			void AllowRollUp(bool allow);

			void RollUp(bool rolled);

			void SetFont(Font& font);
			void SetBackgroundAlpha(float alpha);

			virtual void Update(float deltaTime);

		protected:
			void OnWindowAdded();

			std::string m_title;
			std::string m_hashName;
			int m_hashIdx;

			bool m_allowResize;
			bool m_allowMove;
			bool m_allowRollup;
			bool m_rolledUp;

			Font* m_font;
			float m_backgroundAlpha;

			std::vector<Widget*> m_widgets;

			friend class GUI;
		};
	}
}