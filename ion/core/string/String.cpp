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
#include "core/utils/STL.h"

#include <locale>
#include <algorithm>
#include <cctype>
#include <functional>
#include <vector>
#include <string>

#if !defined ION_PLATFORM_DREAMCAST && !defined ION_PLATFORM_ANDROID
#include <codecvt>
#endif

namespace ion
{
    namespace string
    {
		std::string ToUpper(const std::string& string)
		{
			std::string stringUpper = string;
			std::transform(stringUpper.begin(), stringUpper.end(), stringUpper.begin(), ::toupper);
			return stringUpper;
		}

		std::wstring ToUpper(const std::wstring& string)
		{
			std::wstring stringUpper = string;
			std::transform(stringUpper.begin(), stringUpper.end(), stringUpper.begin(), ::toupper);
			return stringUpper;
		}

        std::string ToLower(const std::string& string)
        {
            std::string stringLower = string;
            std::transform(stringLower.begin(), stringLower.end(), stringLower.begin(), ::tolower);
            return stringLower;
        }

		std::wstring ToLower(const std::wstring& string)
		{
			std::wstring stringLower = string;
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

		size_t FindFirstWhitespace(const std::string& string)
		{
			std::string::const_iterator it = std::find_if(string.begin(), string.end(), ::isspace);
			if (it == string.end())
			{
				return std::string::npos;
			}
			else
			{
				return std::distance(string.begin(), it);
			}
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

		std::string Strip(const std::string& input, const std::vector<char>& delimiters)
		{
			std::string output;

			for (int i = 0; i < input.size(); i++)
			{
				if (std::find(delimiters.begin(), delimiters.end(), input[i]) == delimiters.end())
				{
					output += input[i];
				}
			}

			return output;
		}

#if !defined ION_PLATFORM_DREAMCAST && !defined ION_PLATFORM_ANDROID
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

#if defined UNICODE
		std::wstring StringToNative(const std::string& string)
		{
			return StringToWString(string);
		}

		std::string NativeToString(const std::wstring& string)
		{
			return WStringToString(string);
		}
#else
		std::string StringToNative(const std::string& string)
		{
			return string;
		}

		std::string NativeToString(const std::string& string)
		{
			return string;
		}
#endif

		int Tokenise(const std::string& input, std::vector<std::string>& tokens, const std::string& separator)
		{
			int lastSeparator = 0;

			for (unsigned int i = 0; i < input.size(); i++)
			{
				if (ion::string::StartsWith(input.data() + i, separator))
				{
					tokens.push_back(input.substr(lastSeparator, i - lastSeparator));
					lastSeparator = i + 1;
				}
			}

			tokens.push_back(input.substr(lastSeparator, input.size() - lastSeparator));

			return (int)tokens.size();
		}

        int Tokenise(const std::string& input, std::vector<std::string>& tokens, const std::vector<char>& delimiters)
        {
			int tokenStart = 0;

			for (unsigned int i = 0; i < input.size(); i++)
			{
				while (utils::stl::Find(delimiters, input[i]) && i < input.size())
				{
					i++;
				}

				tokenStart = i;

				while (!utils::stl::Find(delimiters, input[i]) && i < input.size())
				{
					i++;
				}

				tokens.push_back(input.substr(tokenStart, i - tokenStart));
			}

			return (int)tokens.size();
        }

		int TokeniseByWhitespace(const std::string& input, std::vector<std::string>& tokens)
		{
			int tokenStart = 0;

			for (unsigned int i = 0; i < input.size(); i++)
			{
				while (::isspace(input[i]) && i < input.size())
				{
					i++;
				}

				tokenStart = i;

				while (!::isspace(input[i]) && i < input.size())
				{
					i++;
				}

				tokens.push_back(input.substr(tokenStart, i - tokenStart));
			}

			return (int)tokens.size();
		}

        bool CompareNoCase(const std::string& lhs, const std::string& rhs)
        {
            std::string lhsLower = ToLower(lhs);
            std::string rhsLower = ToLower(rhs);
            return lhsLower == rhsLower;
        }

		bool CompareNoCase(const std::wstring& lhs, const std::wstring& rhs)
		{
			std::wstring lhsLower = ToLower(lhs);
			std::wstring rhsLower = ToLower(rhs);
			return lhsLower == rhsLower;
		}

		bool StartsWith(const std::string& string, const std::string& start)
		{
			return string.rfind(start, 0) == 0;
		}

		bool EndsWith(const std::string& string, const std::string& end)
		{
			if (string.length() >= end.length())
			{
				return string.compare(string.length() - end.length(), end.length(), end) == 0;
			}
			
			return false;
		}

#if !defined ION_PLATFORM_DREAMCAST && !defined ION_PLATFORM_ANDROID
		std::string AddNumericPostfix(const std::string& string, int add)
		{
			std::string returnString;

			size_t numStart = -1;

			for (int i = (int)string.size() - 1; i >= 0; i--)
			{
				if (std::isdigit(string[i]))
				{
					numStart = i;
				}
			}

			if (numStart >= 0)
			{
				//Postfix number found, increment and rebuild string
				std::string numText = string.substr(numStart, string.size());
				std::string alphaText = string.substr(0, numStart);
				int num = std::atoi(numText.c_str()) + add;
				returnString = alphaText + std::to_string(num);
			}
			else
			{
				//No postfix number found, append it
				returnString = string + std::to_string(add);
			}

			return returnString;
		}
#endif

		std::string GetFileExtension(const std::string filename)
		{
			size_t dot = filename.find_first_of('.');
			if (dot != std::string::npos && dot != filename.size() - 1)
				return filename.substr(dot + 1, filename.size() - dot - 1);
			return "";
		}
	}
}
