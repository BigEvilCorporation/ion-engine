///////////////////////////////////////////////////
// File:		CommonDlg.cpp
// Date:		25th July 2018
// Authors:		Matt Phillips
// Description:	OS common dialogs
///////////////////////////////////////////////////

#include "CommonDlg.h"

#include <ion/core/Platform.h>
#include <ion/core/string/String.h>

#include <sstream>

#if defined ION_GUI_SUPPORTS_TINYFILEDIALOGS
#include <tinyfiledialogs.h>
#endif

namespace ion
{
	namespace gui
	{
		namespace commondlg
		{
			std::string DirectoryDialog(render::Window* parent, const std::string& title, const std::string& defaultPath)
			{
#if defined ION_PLATFORM_WINDOWS
				IFileDialog* fileDialog = nullptr;
				std::string directory;

				if (CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&fileDialog)) >= 0)
				{
					DWORD options;

					if (fileDialog->GetOptions(&options) >= 0)
					{
						fileDialog->SetOptions(options | FOS_PICKFOLDERS);
					}

					if (parent)
					{
						RedrawWindow(parent->GetWindowHandle(), 0, 0, RDW_INTERNALPAINT);
					}

					if (fileDialog->Show(parent ? parent->GetWindowHandle() : nullptr) >= 0)
					{
						IShellItem* shellItem = nullptr;

						if (fileDialog->GetResult(&shellItem) >= 0)
						{
							PWSTR wdirectory;

							if (shellItem->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &wdirectory) >= 0)
							{
								directory = string::WStringToString(wdirectory);
								CoTaskMemFree(wdirectory);
							}

							shellItem->Release();
						}
					}

					fileDialog->Release();

					return directory;
				}
#elif defined ION_GUI_SUPPORTS_TINYFILEDIALOGS
				const char* directory = tinyfd_selectFolderDialog(title.c_str(), defaultPath.c_str());

				if (directory)
					return directory;
				else
					return "";
#else
				debug::Error("ion::gui::commondlg::DirectoryDialog() - Not implemented on this platform");
				return "";
#endif
			}

			std::string FileSaveDialog(render::Window* parent, const std::string& title, const std::vector<std::string>& filter, const std::string& fileTypeName, const std::string& defaultPath)
			{
#if defined ION_GUI_SUPPORTS_TINYFILEDIALOGS
				const int maxPatterns = 32;
				const int maxStringLen = 32;
				char patterns[maxPatterns][maxStringLen];
				char* patternArr[maxPatterns];

				for (int i = 0; i < filter.size() && i < maxPatterns; i++)
				{
					sprintf_s(patterns[i], maxStringLen, "{*.%s}", filter[i].c_str());
					patternArr[i] = patterns[i];
				}

				const char* filename = tinyfd_saveFileDialog(title.c_str(), defaultPath.c_str(), filter.size(), patternArr, fileTypeName.c_str());

				if (filename)
					return filename;
				else
					return "";
#else
				debug::Error("ion::gui::commondlg::DirectoryDialog() - Not implemented on this platform");
				return "";
#endif
			}
		}
	}
}