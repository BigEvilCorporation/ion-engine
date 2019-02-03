///////////////////////////////////////////////////
// File:		CommonDlg.h
// Date:		25th July 2018
// Authors:		Matt Phillips
// Description:	OS common dialogs
///////////////////////////////////////////////////

#pragma once

#include <ion/renderer/Window.h>

#include <string>
#include <tuple>
#include <vector>

namespace ion
{
	namespace gui
	{
		namespace commondlg
		{
			std::string DirectoryDialog(render::Window* parent, const std::string& title, const std::string& defaultPath);
			std::string FileSaveDialog(render::Window* parent, const std::string& title, const std::vector<std::string>& filter, const std::string& fileTypeName, const std::string& defaultPath);
		}
	}
}