///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		ShaderGLSL.h
// Date:		11th December 2013
// Authors:		Matt Phillips
// Description:	GLSL shader implementation
///////////////////////////////////////////////////

#pragma once

#include "renderer/Shader.h"
#include "renderer/opengl/OpenGLInclude.h"

namespace ion
{
	namespace render
	{
		class ShaderManagerGLSL : public ShaderManager
		{
		public:
			ShaderManagerGLSL();
			virtual ~ShaderManagerGLSL();
		};

		class ShaderGLSL : public Shader
		{
		public:
			ShaderGLSL();
			virtual ~ShaderGLSL();

			//Compile shader
			virtual bool Compile();

			//Bind/unbind
			virtual void Bind();
			virtual void Unbind();

			static const std::string s_languageName;

		protected:
			class ShaderParamDelegateGLSL : public ShaderParamDelegate
			{
			public:

				ShaderParamDelegateGLSL(GLint param) { m_param = param; }

				virtual void Set(const int& value);
				virtual void Set(const float& value);
				virtual void Set(const Vector2& value);
				virtual void Set(const Vector3& value);
				virtual void Set(const Colour& value);
				virtual void Set(const Matrix4& value);
				virtual void Set(const Texture& value);

				virtual void Set(const std::vector<float>& values);
				virtual void Set(const std::vector<Colour>& values);

				GLint m_param;
				static int s_textureBindCount;
			};

			struct GLSLShader
			{
				GLenum program;
				GLenum profile;
			};

			virtual ShaderParamDelegate* CreateShaderParamDelegate(const std::string& paramName);

			void Destroy(std::vector<GLSLShader>& glShaders);
			bool CheckCompilerError(Program& program, GLenum glShader);
			bool ValidateProgram(GLenum glProgram);
			bool CheckLinkerError(GLenum glProgram);

			GLenum m_glProgram;
			int m_bindCount;
		};
	}
}