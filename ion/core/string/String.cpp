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

#include <algorithm>
#include <vector>

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