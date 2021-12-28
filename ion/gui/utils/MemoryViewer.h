///////////////////////////////////////////////////
// File:		MemoryViewer.h
// Date:		4th February 2019
// Authors:		Matt Phillips
// Description:	Memory Viewer widget
///////////////////////////////////////////////////

#pragma once

#include "../Widget.h"

#include <string>
#include <functional>
#include <vector>

#include <ion/dependencies/imgui/imgui.h>
#include <ion/dependencies/imgui/imgui_memory_editor/imgui_memory_editor.h>

namespace ion
{
	namespace gui
	{
		class MemoryViewer : public Widget
		{
		public:
			MemoryViewer(std::vector<u8>& memory, u32 baseAddress);

			virtual void Update(float deltaTime);

		private:
			std::vector<u8>& m_memory;
			MemoryEditor m_editor;
			u32 m_baseAddress;
		};
	}
}