///////////////////////////////////////////////////
// File:		TextEditor.cpp
// Date:		9th February 2019
// Authors:		Matt Phillips
// Description:	Memory Viewer widget
///////////////////////////////////////////////////

#include "TextEditor.h"

namespace ion
{
	namespace gui
	{
		TextEditor::TextEditor()
		{
		}

		void TextEditor::SetText(const std::string& text)
		{
			m_editor.SetText(text);
		}

		void TextEditor::HighlightLine(int line)
		{
			::TextEditor::Coordinates coords;
			coords.mColumn = 0;
			coords.mLine = line;
			m_editor.SetCursorPosition(coords);
		}

		void TextEditor::Update(float deltaTime)
		{
			if (m_visible)
			{
				m_editor.Render("");
			}
		}
	}
}