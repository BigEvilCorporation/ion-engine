///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Debug.cpp
// Date:		25th July 2011
// Authors:		Matt Phillips
// Description:	Debug output, log
///////////////////////////////////////////////////

#include "Debug.h"
#include "ion/core/Types.h"
#include "ion/core/Platform.h"
#include "CrashHandler.h"

namespace ion
{
	namespace debug
	{
		LogStream log(LogStream::Type::Log);
		LogStream error(LogStream::Type::Error);
		LogTokenEnd end;

		LogStream& LogStream::operator << (const char* text)
		{
#if !defined ION_BUILD_MASTER
			m_stream << text;
#endif
			return *this;
		}

		LogStream& LogStream::operator << (const std::string& text)
		{
#if !defined ION_BUILD_MASTER
			m_stream << text;
#endif
			return *this;
		}

		LogStream& LogStream::operator << (u8 number)
		{
#if !defined ION_BUILD_MASTER
			m_stream << (int)number;
#endif
			return *this;
		}

		LogStream& LogStream::operator << (s8 number)
		{
#if !defined ION_BUILD_MASTER
			m_stream << (int)number;
#endif
			return *this;
		}

		LogStream& LogStream::operator << (u16 number)
		{
#if !defined ION_BUILD_MASTER
			m_stream << (int)number;
#endif
			return *this;
		}

		LogStream& LogStream::operator << (s16 number)
		{
#if !defined ION_BUILD_MASTER
			m_stream << (int)number;
#endif
			return *this;
		}

		LogStream& LogStream::operator << (u32 number)
		{
#if !defined ION_BUILD_MASTER
			m_stream << (int)number;
#endif
			return *this;
		}

		LogStream& LogStream::operator << (s32 number)
		{
#if !defined ION_BUILD_MASTER
			m_stream << (int)number;
#endif
			return *this;
		}

		LogStream& LogStream::operator << (u64 number)
		{
#if !defined ION_BUILD_MASTER
			m_stream << (long long)number;
#endif
			return *this;
		}

		LogStream& LogStream::operator << (s64 number)
		{
#if !defined ION_BUILD_MASTER
			m_stream << (long long)number;
#endif
			return *this;
		}

		LogStream& LogStream::operator << (float number)
		{
#if !defined ION_BUILD_MASTER
			m_stream << number;
#endif
			return *this;
		}

		LogStream& LogStream::operator << (LogTokenEnd token)
		{
#if !defined ION_BUILD_MASTER
			if(m_type == Type::Log)
			{
				Log(m_stream.str().c_str());
			}
			else if(m_type == Type::Error)
			{
				Error(m_stream.str().c_str());
			}

			m_stream.str(std::string());
#endif

			return *this;
		}
	}
}
