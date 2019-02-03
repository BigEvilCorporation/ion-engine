///////////////////////////////////////////////////
// File:		ComboBox.cpp
// Date:		12th November 2011
// Authors:		Matt Phillips
// Description:	UI list box
///////////////////////////////////////////////////

#include "ComboBox.h"

#include <ion/core/debug/Debug.h>
#include <ion/dependencies/imgui/imgui.h>
#include <ion/dependencies/imgui/imgui_internal.h>

namespace ion
{
	namespace gui
	{
		ComboBox::Item::Item(const std::string& text, int id)
			: m_text(text)
			, m_id(id)
		{
		}

		const std::string& ComboBox::Item::GetText() const
		{
			return m_text;
		}

		int ComboBox::Item::GetId() const
		{
			return m_id;
		}

		ComboBox::ComboBox(const std::string& text, std::function<void(const ComboBox&, const Item&)> const& onSelected)
			: m_text(text)
			, m_onSelected(onSelected)
		{
			m_selected = 0;
		}

		ComboBox::~ComboBox()
		{
		}

		void ComboBox::SetSelection(int index)
		{
			debug::Assert(index >= 0 && index < m_items.size(), "ComboBox::SetSelection() - Selection out of bounds");
			m_selected = index;
		}

		int ComboBox::GetSelection() const
		{
			return m_selected;
		}

		void ComboBox::AddItem(const ComboBox::Item& item)
		{
			m_items.push_back(item);
		}

		void ComboBox::RemoveItem(const ComboBox::Item& item)
		{
			m_items.erase(std::remove(m_items.begin(), m_items.end(), item), m_items.end());
		}

		void ComboBox::Update(float deltaTime)
		{
			if (m_visible)
			{
				std::vector<const char*> itemNames;

				for (int i = 0; i < m_items.size(); i++)
				{
					itemNames.push_back(m_items[i].GetText().c_str());
				}

				int prevSelected = m_selected;

				if (m_arrangement == Arrangement::Horizontal)
				{
					ImGui::SameLine();
				}

				if (!m_enabled)
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}

				ImGui::Combo(m_text.c_str(), &m_selected, itemNames.data(), itemNames.size());

				if (!m_enabled)
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				if (m_selected != prevSelected)
				{
					m_onSelected(*this, m_items[m_selected]);
				}
			}
		}
	}
}