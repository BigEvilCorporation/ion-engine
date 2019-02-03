///////////////////////////////////////////////////
// File:		ListBox.h
// Date:		12th November 2011
// Authors:		Matt Phillips
// Description:	UI list box
///////////////////////////////////////////////////

#pragma once

#include "Widget.h"

#include <string>
#include <vector>

namespace ion
{
	namespace gui
	{
		class ListBox : public Widget
		{
		public:
			class Item
			{
			public:
				Item(std::string text, int id);
				std::string GetText() const;
				int GetId() const;
			};

			ListBox();
			~ListBox();

			void AddItem(Item& item);
			void RemoveItem(Item& item);
			int GetSelectedItems(std::list<Item*>& itemList);
		};
	}
}