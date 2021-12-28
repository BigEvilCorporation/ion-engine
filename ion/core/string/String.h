///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		String.h
// Date:		16th February 2014
// Authors:		Matt Phillips
// Description:	String utils
///////////////////////////////////////////////////

#pragma once

#include <string>
#include <vector>

namespace ion
{
#if defined UNICODE
	typedef wchar_t char_t;
	typedef std::wstring string_t;
#else
	typedef char char_t;
	typedef std::string string_t;
#endif

	namespace string
	{
		std::string ToUpper(const std::string& string);
		std::wstring ToUpper(const std::wstring& string);
		std::string ToLower(const std::string& string);
		std::wstring ToLower(const std::wstring& string);

		std::string TrimWhitespaceStart(const std::string& string);
		std::string TrimWhitespaceEnd(const std::string& string);

		size_t FindFirstWhitespace(const std::string& string);

		std::string RemoveSubstring(const std::string& string, const std::string& substring);

		std::string Strip(const std::string& input, const std::vector<char>& delimiters);

#if !defined ION_PLATFORM_DREAMCAST
		std::string WStringToString(const std::wstring& string);
		std::wstring StringToWString(const std::string& string);
#endif

#if defined UNICODE
		std::wstring StringToNative(const std::string& string);
		std::string NativeToString(const std::wstring& string);
#else
		std::string StringToNative(const std::string& string);
		std::string NativeToString(const std::string& string);
#endif

        int Tokenise(const std::string& input, std::vector<std::string>& tokens, const std::string& separator);
        int Tokenise(const std::string& input, std::vector<std::string>& tokens, const std::vector<char>& delimiters);
		int TokeniseByWhitespace(const std::string& input, std::vector<std::string>& tokens);
        
        bool CompareNoCase(const std::string& lhs, const std::string& rhs);
		bool CompareNoCase(const std::wstring& lhs, const std::wstring& rhs);
		bool StartsWith(const std::string& string, const std::string& start);
		bool EndsWith(const std::string& string, const std::string& end);

		//Adds to a number at the end of a string
		std::string AddNumericPostfix(const std::string& string, int add);

		//Filenames and paths
		std::string GetFileExtension(const std::string filename);
	}
}
