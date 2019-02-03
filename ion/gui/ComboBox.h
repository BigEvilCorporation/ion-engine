///////////////////////////////////////////////////
// File:		ComboBox.h
// Date:		12th November 2011
// Authors:		Matt Phillips
// Description:	UI list box
///////////////////////////////////////////////////

#pragma once

#include "Widget.h"

#include <string>
#include <vector>
#include <functional>

namespace ion
{
	namespace gui
	{
		class ComboBox : public Widget
		{
		public:
			class Item
			{
			public:
				Item(const std::string& text, int id);
				const std::string& GetText() const;
				int GetId() const;

				bool operator == (const Item& rhs) const { return m_id == rhs.GetId(); }
			private:
				std::string m_text;
				int m_id;
			};

			ComboBox(const std::string& text, std::function<void(const ComboBox&, const Item&)> const& onSelected);
			~ComboBox();

			void SetSelection(int index);
			int GetSelection() const;

			void AddItem(const Item& item);
			void RemoveItem(const Item& item);

			virtual void Update(float deltaTime);

		private:
			std::string m_text;
			std::function<void(const ComboBox&, const Item&)> m_onSelected;
			std::vector<Item> m_items;
			int m_selected;
		};
	}
}