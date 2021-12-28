///////////////////////////////////////////////////
// File:		MemoryViewer.cpp
// Date:		4th February 2019
// Authors:		Matt Phillips
// Description:	Memory Viewer widget
///////////////////////////////////////////////////

#include "MemoryViewer.h"

namespace ion
{
	namespace gui
	{
		MemoryViewer::MemoryViewer(std::vector<u8>& memory, u32 baseAddress)
			:  m_memory(memory)
			, m_baseAddress(baseAddress)
		{
		}

		void MemoryViewer::Update(float deltaTime)
		{
			if (m_visible)
			{
				m_editor.DrawContents(m_memory.data(), m_memory.size(), m_baseAddress);
			}
		}
	}
}