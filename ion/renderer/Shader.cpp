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

		void Shader::SetProgram(const std::string& name, const std::string& programCode, const std::string& entryPoint, ProgramType programType)
		{
			m_name = name;
			m_programCode = programCode;
			m_entryPoint = entryPoint;
			m_programType = programType;
		}

		void Shader::Serialise(io::Archive& archive)
		{
			archive.Serialise(m_name, "name");
			archive.Serialise(m_programCode, "programCode");
			archive.Serialise(m_entryPoint, "entryPoint");
			archive.Serialise((u32&)m_programType, "programType");

			if(archive.GetDirection() == io::Archive::Direction::In)
			{
				if(archive.GetResourceManager())
				{
					if (!Compile())
					{
						ion::debug::log << "Failed to compile shader " << m_name << ion::debug::end;
					}
				}
			}
		}
	}
}