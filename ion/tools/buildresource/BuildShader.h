///////////////////////////////////////////////////
// File:		BuildShader.h
// Date:		15th February 2014
// Authors:		Matt Phillips
// Description:	Resource build tool
///////////////////////////////////////////////////

#pragma once

#include "renderer/Shader.h"

#include <string>

namespace ion
{
	namespace build
	{
		struct ShaderProgram
		{
			render::Shader::ProgramType programType;
			std::string language;
			std::string entrypoint;
			std::string sourceFilename;
		};

		int BuildShader(const std::string& outputFilename, const std::vector<ShaderProgram>& programs);
	}

	namespace render
	{
		class ShaderResource : public Shader
		{
		public:
			virtual bool Compile() { return true; }
			virtual void Bind() {}
			virtual void Unbind() {}
		protected:
			virtual ShaderParamDelegate* CreateShaderParamDelegate(const std::string& paramName) { return nullptr; }
		};
	}
}