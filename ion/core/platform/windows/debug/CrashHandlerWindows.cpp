///////////////////////////////////////////////////
// (c) 2020 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Debug.cpp
// Date:		27th August 2020
// Authors:		Matt Phillips
// Description:	Crash dump handler
///////////////////////////////////////////////////

#include "core/Platform.h"
#include "core/debug/Debug.h"
#include "core/debug/CrashHandler.h"

namespace ion
{
	namespace debug
	{
		void Win32GenerateMiniDump(EXCEPTION_POINTERS* e)
		{
			auto hDbgHelp = LoadLibraryA("dbghelp");
			if (hDbgHelp == nullptr)
				return;
			auto pMiniDumpWriteDump = (decltype(&MiniDumpWriteDump))GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
			if (pMiniDumpWriteDump == nullptr)
				return;

			char name[MAX_PATH];
			{
				auto nameEnd = name + GetModuleFileNameA(GetModuleHandleA(0), name, MAX_PATH);
				SYSTEMTIME t;
				GetSystemTime(&t);
				wsprintfA(nameEnd - strlen(".exe"),
					"_%4d%02d%02d_%02d%02d%02d.dmp",
					t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond);
			}

			auto hFile = CreateFileA(name, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
			if (hFile == INVALID_HANDLE_VALUE)
				return;

			MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
			exceptionInfo.ThreadId = GetCurrentThreadId();
			exceptionInfo.ExceptionPointers = e;
			exceptionInfo.ClientPointers = FALSE;

			auto dumped = pMiniDumpWriteDump(
				GetCurrentProcess(),
				GetCurrentProcessId(),
				hFile,
				MINIDUMP_TYPE(MiniDumpWithIndirectlyReferencedMemory | MiniDumpScanMemory),
				e ? &exceptionInfo : nullptr,
				nullptr,
				nullptr);

			CloseHandle(hFile);

			return;
		}

		LONG CALLBACK Win32CrashHandler(EXCEPTION_POINTERS* e)
		{
			Win32GenerateMiniDump(e);
			return EXCEPTION_CONTINUE_SEARCH;
		}

		void InstallDefaultCrashHandler()
		{
			SetUnhandledExceptionFilter(Win32CrashHandler);
		}

		void PrintCallstack()
		{
		}
	}
}