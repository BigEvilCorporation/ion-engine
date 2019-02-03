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
	namespace string
	{
        std::string ToLower(const std::string& string);

		std::string TrimWhitespaceStart(const std::string& string);
		std::string TrimWhitespaceEnd(const std::string& string);

		std::string RemoveSubstring(const std::string& string, const std::string& substring);

#if !defined ION_PLATFORM_DREAMCAST
		std::string WStringToString(const std::wstring& string);
		std::wstring StringToWString(const std::string& string);
#endif

        int Tokenise(const std::string input, std::vector<std::string>& tokens, char separator);
        
        bool CompareNoCase(const std::string& lhs, const std::string& rhs);
	}
}
