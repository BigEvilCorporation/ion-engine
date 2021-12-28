///////////////////////////////////////////////////
// File:		TextEditor.h
// Date:		9th February 2019
// Authors:		Matt Phillips
// Description:	Memory Viewer widget
///////////////////////////////////////////////////

#pragma once

#include "../Widget.h"

#include <string>
#include <functional>
#include <vector>

#include <ion/dependencies/imgui/imgui.h>
#include <ion/dependencies/imgui/ImGuiColorTextEdit/TextEditor.h>

namespace ion
{
	namespace gui
	{
		class TextEditor : public Widget
		{
		public:
			TextEditor();

			void SetText(const std::string& text);
			void HighlightLine(int line);

			virtual void Update(float deltaTime);

		private:
			::TextEditor m_editor;
		};
	}
}