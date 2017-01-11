///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Debug.h
// Date:		25th July 2011
// Authors:		Matt Phillips
// Description:	Debug output, log
///////////////////////////////////////////////////

#pragma once

#include "core/Types.h"
#include <sstream>
#include <string>

namespace ion
{
	namespace debug
	{
		void Log(const char* message);
		void Flush();
		void Error(const char* message);
		void Assert(bool condition, const char* message);
		void Break();

		void PrintMemoryUsage();

		struct LogTokenEnd {};

		class LogStream
		{
		public:
			LogStream& operator << (const char* text);
			LogStream& operator << (const std::string& text);
			LogStream& operator << (u8 number);
			LogStream& operator << (s8 number);
			LogStream& operator << (u16 number);
			LogStream& operator << (s16 number);
			LogStream& operator << (u32 number);
			LogStream& operator << (s32 number);
			LogStream& operator << (float number);
			LogStream& operator << (LogTokenEnd token);

		private:
			//TODO: Store one per thread in TLS
			std::stringstream m_stream;
		};

		static LogStream log;
		static LogTokenEnd end;
	}
}