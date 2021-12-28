///////////////////////////////////////////////////
// File:		Platform.h
// Date:		26th January 2017
// Authors:		Matt Phillips
// Description:	Platform includes, types, macros
///////////////////////////////////////////////////

#include "Windows.h"
#include "core/Platform.h"
#include "core/debug/Debug.h"
#include "core/cryptography/UUID.h"
#include "core/debug/CrashHandler.h"

#include <iostream>
#include <shellapi.h>

namespace ion
{
	namespace platform
	{
		void Initialise()
		{

		}

		void Shutdown()
		{

		}

		Language GetSystemLanguage()
		{
			return Language::BritishEnglish;
		}

		void RegisterCallbackSystemMenu(SystemMenuCallback const& callback)
		{

		}

		std::string GetLastWin32Error()
		{
			DWORD errorCode = GetLastError();
			char* messageBuff;
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&messageBuff, 0, NULL);
			std::string output = messageBuff;
			LocalFree(messageBuff);
			return output;
		}
	}
}

#if defined ION_BUILD_MASTER
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int numargs)
{
	return ion::EntryPoint(numargs, __argv);
}
#else
int main(int numargs, char** args)
{
	return ion::EntryPoint(numargs, args);
}
#endif