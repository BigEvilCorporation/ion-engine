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
        void InstallDefaultCrashHandler()
        {
        }

		void PrintCallstack()
		{
			void DreamcastAssertHandler(const char* file, int line, const char* expr, const char* msg, const char* func)
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

			void DreamcastExceptionHandler(irq_t code, irq_context_t * context)
			{
				dbgio_printf("EXCEPTION\n");
				PrintCallstack();
				while (1) {}
			}

			void InstallDefaultCrashHandler()
			{
				assert_set_handler(DreamcastAssertHandler);
				irq_set_handler(EXC_ILLEGAL_INSTR, DreamcastExceptionHandler);
				irq_set_handler(EXC_SLOT_ILLEGAL_INSTR, DreamcastExceptionHandler);
				irq_set_handler(EXC_DATA_ADDRESS_READ, DreamcastExceptionHandler);
				irq_set_handler(EXC_DATA_ADDRESS_WRITE, DreamcastExceptionHandler);
				irq_set_handler(EXC_USER_BREAK_PRE, DreamcastExceptionHandler);
			}

			void PrintCallstack()
			{
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
			}
		}
	}
}