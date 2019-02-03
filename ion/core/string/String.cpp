///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		String.cpp
// Date:		16th February 2014
// Authors:		Matt Phillips
// Description:	String utils
///////////////////////////////////////////////////

#include "String.h"

#include <locale>
#include <algorithm>
#include <cctype>
#include <functional>
#include <vector>

#if !defined ION_PLATFORM_DREAMCAST
#include <codecvt>
#endif

namespace ion
{
    namespace string
    {
        std::string ToLower(const std::string& string)
        {
            std::string stringLower = string;
            std::transform(stringLower.begin(), stringLower.end(), stringLower.begin(), ::tolower);
            return stringLower;
        }

		std::string TrimWhitespaceStart(const std::string& string)
		{
			std::string output = string;
			output.erase(output.begin(), std::find_if(output.begin(), output.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
			return output;
		}

		std::string TrimWhitespaceEnd(const std::string& string)
		{
			std::string output = string;
			output.erase(std::find_if(output.rbegin(), output.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), output.end());
			return output;
		}

		std::string RemoveSubstring(const std::string& string, const std::string& substring)
		{
			std::string output = string;
			size_t pos = output.find(substring);

			if (pos != std::string::npos)
			{
				output.erase(pos, substring.size());
			}

			return output;
		}

#if !defined ION_PLATFORM_DREAMCAST
		std::string WStringToString(const std::wstring& string)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			return converter.to_bytes(string);
		}

		std::wstring StringToWString(const std::string& string)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			return converter.from_bytes(string);
		}
#endif

        int Tokenise(const std::string input, std::vector<std::string>& tokens, char separator)
        {
            int lastSeparator = 0;
            
            for(unsigned int i = 0; i < input.size(); i++)
            {
                if(input[i] == separator)
                {
                    tokens.push_back(input.substr(lastSeparator, i - lastSeparator));
                    lastSeparator = i +	1;
                }
            }
            
            tokens.push_back(input.substr(lastSeparator, input.size() - lastSeparator));
            
            return tokens.size();
        }

        bool CompareNoCase(const std::string& lhs, const std::string& rhs)
        {
            std::string lhsLower = ToLower(lhs);
            std::string rhsLower = ToLower(rhs);
            return lhsLower == rhsLower;
        }
    }
}
