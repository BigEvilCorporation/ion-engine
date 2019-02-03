///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		CrashHandler.cpp
// Date:		16th August 2018
// Authors:		Matt Phillips
// Description:	Crash dump handler
///////////////////////////////////////////////////

#include "CrashHandler.h"

#include <core/Platform.h>
#include <core/debug/Debug.h>
#include <core/utils/STL.h>

namespace ion
{
	namespace debug
	{

#if defined ION_PLATFORM_WINDOWS
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
#endif

#if defined ION_PLATFORM_DREAMCAST
		void DreamcastAssertHandler(const char * file, int line, const char * expr, const char * msg, const char * func)
		{
			error << "ASSERT: \n"
				<< "file: " << file << "\n"
				<< "line: " << line << "\n"
				<< "expr: " << expr << "\n"
				<< "msg: " << msg << "\n"
				<< "func: " << func << "\n"
				<< end;

			PrintCallstack();
		}

		void DreamcastExceptionHandler(irq_t code, irq_context_t *context)
		{
			dbgio_printf("EXCEPTION\n");
			PrintCallstack();
			while (1) {}
		}
#endif

		void InstallDefaultCrashHandler()
		{
#if defined ION_PLATFORM_WINDOWS
			SetUnhandledExceptionFilter(Win32CrashHandler);
#elif defined ION_PLATFORM_DREAMCAST
			assert_set_handler(DreamcastAssertHandler);
			irq_set_handler(EXC_ILLEGAL_INSTR, DreamcastExceptionHandler);
			irq_set_handler(EXC_SLOT_ILLEGAL_INSTR, DreamcastExceptionHandler);
			irq_set_handler(EXC_DATA_ADDRESS_READ, DreamcastExceptionHandler);
			irq_set_handler(EXC_DATA_ADDRESS_WRITE, DreamcastExceptionHandler);
			irq_set_handler(EXC_USER_BREAK_PRE, DreamcastExceptionHandler);
#endif
		}

		void PrintCallstack()
		{
#if defined ION_PLATFORM_DREAMCAST
			dbgio_printf("CALLSTACK:\n");

			u32 framePtr = arch_get_fptr();

			while (framePtr != 0xffffffff)
			{
				if (!arch_valid_address(framePtr))
				{
					dbgio_printf("(Invalid frame pointer)\n");
					break;
				}

				printf("0x%08x\n", arch_fptr_ret_addr(framePtr));

				framePtr = arch_fptr_next(framePtr);
			}
#elif defined ION_PLATFORM_LINUX
            const int max_frames = 63;
            
            // storage array for stack trace address data
            void* addrlist[max_frames+1];

            // retrieve current stack addresses
            int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));

            if (addrlen == 0)
            {
                printf("  <empty, possibly corrupt>\n");
                return;
            }

            // resolve addresses into strings containing "filename(function+address)",
            // this array must be free()-ed
            char** symbollist = backtrace_symbols(addrlist, addrlen);

            // allocate string which will be filled with the demangled function name
            size_t funcnamesize = 256;
            char* funcname = (char*)malloc(funcnamesize);

            // iterate over the returned symbol lines. skip the first, it is the
            // address of this function.
            for (int i = 1; i < addrlen; i++)
            {
                char *begin_name = 0, *begin_offset = 0, *end_offset = 0;

                // find parentheses and +address offset surrounding the mangled name:
                // ./module(function+0x15c) [0x8048a6d]
                for (char *p = symbollist[i]; *p; ++p)
                {
                    if (*p == '(')
                        begin_name = p;
                    else if (*p == '+')
                        begin_offset = p;
                    else if (*p == ')' && begin_offset)
                    {
                        end_offset = p;
                        break;
                    }
                }

                if (begin_name && begin_offset && end_offset
                    && begin_name < begin_offset)
                {
                    *begin_name++ = '\0';
                    *begin_offset++ = '\0';
                    *end_offset = '\0';

                    // mangled name is now in [begin_name, begin_offset) and caller
                    // offset in [begin_offset, end_offset). now apply
                    // __cxa_demangle():

                    int status;
                    char* ret = abi::__cxa_demangle(begin_name,
                                    funcname, &funcnamesize, &status);
                    if (status == 0)
                    {
                        funcname = ret; // use possibly realloc()-ed string
                        printf("  %s : %s+%s\n",
                        symbollist[i], funcname, begin_offset);
                    }
                    else
                        {
                    // demangling failed. Output function name as a C function with
                        // no arguments.
                        printf("  %s : %s()+%s\n",
                        symbollist[i], begin_name, begin_offset);
                    }
                }
                else
                {
                    // couldn't parse the line? print the whole line.
                    printf("  %s\n", symbollist[i]);
                }
            }

            free(funcname);
            free(symbollist);
#endif
		}
	}
}
