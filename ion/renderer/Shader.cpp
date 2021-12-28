///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Shader.cpp
// Date:		8th December 2013
// Authors:		Matt Phillips
// Description:	Shader base class
///////////////////////////////////////////////////

#include "renderer/Shader.h"

namespace ion
{
	namespace render
	{
		Shader::Shader()
		{

		}

		Shader::~Shader()
		{

		}

		void Shader::SetProgram(const std::string& language, const std::string& programCode, const std::string& entryPoint, ProgramType programType)
		{
			m_programs[language][(int)programType].m_entryPoint = entryPoint;
			m_programs[language][(int)programType].m_programCode = programCode;
		}

		Shader::Program* Shader::GetProgram(const std::string& language, ProgramType programType)
		{
			std::map<std::string, std::map<int, Program>>::iterator languageIt = m_programs.find(language);
			if (languageIt != m_programs.end())
			{
				std::map<int, Program>::iterator typeIt = languageIt->second.find((int)programType);
				if (typeIt != languageIt->second.end())
				{
					return &typeIt->second;
				}
			}

			return nullptr;
		}

		void Shader::Serialise(io::Archive& archive)
		{
			archive.Serialise(m_programs, "programs");

			if(archive.GetDirection() == io::Archive::Direction::In)
			{
				if(archive.GetResourceManager())
				{
					Compile();
				}
			}
		}
	}
}