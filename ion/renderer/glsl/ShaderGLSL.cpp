///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		ShaderGLSL.cpp
// Date:		11th December 2013
// Authors:		Matt Phillips
// Description:	GLSL shader implementation
///////////////////////////////////////////////////

#include "core/debug/Debug.h"
#include "core/string/String.h"
#include "core/io/File.h"
#include "core/io/Archive.h"
#include "renderer/opengl/TextureOpenGL.h"
#include "renderer/opengl/RendererOpenGL.h"
#include "renderer/opengl/OpenGLExtensions.h"
#include "renderer/GLSL/ShaderGLSL.h"

#define ION_GLSL_CHECK_EXT(name) debug::Assert(opengl::extensions->name != nullptr, "ShaderManagerGLSL() - GLSL requires the " #name " GL extension")

namespace ion
{
	namespace render
	{
		const std::string ShaderGLSL::s_languageName = "glsl";
		int ShaderGLSL::ShaderParamDelegateGLSL::s_textureBindCount = 0;

		ShaderManager* ShaderManager::Create()
		{
			return new ShaderManagerGLSL();
		}

		ShaderManagerGLSL::ShaderManagerGLSL()
		{
			ION_GLSL_CHECK_EXT(glAttachObject);
			ION_GLSL_CHECK_EXT(glDeleteObject);
			ION_GLSL_CHECK_EXT(glCompileShader);
			ION_GLSL_CHECK_EXT(glCreateShader);
			ION_GLSL_CHECK_EXT(glCreateProgram);
			ION_GLSL_CHECK_EXT(glValidateProgram);
			ION_GLSL_CHECK_EXT(glLinkProgram);
			ION_GLSL_CHECK_EXT(glUseProgram);
			ION_GLSL_CHECK_EXT(glShaderSource);
			ION_GLSL_CHECK_EXT(glGetUniformLocation);
			ION_GLSL_CHECK_EXT(glGetShaderiv);
			ION_GLSL_CHECK_EXT(glGetShaderInfoLog);
			ION_GLSL_CHECK_EXT(glGetProgramiv);
			ION_GLSL_CHECK_EXT(glGetProgramInfoLog);
			ION_GLSL_CHECK_EXT(glVertexAttribPointer);
			ION_GLSL_CHECK_EXT(glEnableVertexAttribArray);
			ION_GLSL_CHECK_EXT(glDisableVertexAttribArray);
			ION_GLSL_CHECK_EXT(glUniformMatrix4fv);
			ION_GLSL_CHECK_EXT(glUniform1i);
			ION_GLSL_CHECK_EXT(glUniform4f);
		}

		ShaderManagerGLSL::~ShaderManagerGLSL()
		{

		}

		Shader* Shader::Create()
		{
			return new ShaderGLSL();
		}

		void Shader::RegisterSerialiseType(io::Archive& archive)
		{
			archive.RegisterPointerTypeStrict<Shader, ShaderGLSL>("ion::render::Shader");
		}

		ShaderGLSL::ShaderGLSL()
		{
			m_bindCount = 0;
		}

		ShaderGLSL::~ShaderGLSL()
		{
			OpenGLContextStackLock lock;
			opengl::extensions->glDeleteObject(m_glProgram);
			RendererOpenGL::CheckGLError("ShaderGLSL::~ShaderGLSL() - Error destroying shader");
		}

		bool ShaderGLSL::Compile()
		{
			OpenGLContextStackLock lock;

			//Create combined program
			m_glProgram = opengl::extensions->glCreateProgram();

			//Compile all shaders
			std::vector<GLSLShader> glShaders;

			for (int i = 0; i < (int)ProgramType::COUNT; i++)
			{
				if (Program* program = GetProgram(ion::string::ToLower(s_languageName), (ProgramType)i))
				{
					GLSLShader glShader;

					switch ((ProgramType)i)
					{
					case ProgramType::Vertex:
						glShader.profile = GL_VERTEX_SHADER;
						break;
					case ProgramType::Fragment:
						glShader.profile = GL_FRAGMENT_SHADER;
						break;
					default:
						ion::debug::Error("ShaderGLSL::Compile() - Program type unsupported");
					}

					//Compile program
					glShader.program = opengl::extensions->glCreateShader(glShader.profile);
					const char* programCode[1] = { program->m_programCode.c_str() };
					opengl::extensions->glShaderSource(glShader.program, 1, programCode, NULL);
					opengl::extensions->glCompileShader(glShader.program);
					
					if (!CheckCompilerError(*program, glShader.program))
					{
						//Error, delete shaders compiled so far
						Destroy(glShaders);

						//Delete this shader
						opengl::extensions->glDeleteObject(glShader.program);

						return false;
					}
					
					//Combine program
					opengl::extensions->glAttachObject(m_glProgram, glShader.program);
					RendererOpenGL::CheckGLError("ShaderGLSL::Compile() - Error combining shaders");
					
					if (glShader.program)
					{
						glShaders.push_back(glShader);
					}
				}
			}

			//Link combined program
			if (glShaders.size() > 0)
			{
				opengl::extensions->glLinkProgram(m_glProgram);

				//Check
				if (!CheckLinkerError(m_glProgram))
				{
					Destroy(glShaders);
					return false;
				}
			}

			//Delete individual shaders
			for (int i = 0; i < glShaders.size(); i++)
			{
				opengl::extensions->glDeleteObject(glShaders[i].program);
			}

			//Validate program
			if (!ValidateProgram(m_glProgram))
			{
				Destroy(glShaders);
				return false;
			}

			return glShaders.size() != 0;
		}

		void ShaderGLSL::Destroy(std::vector<GLSLShader>& glShaders)
		{
			//Error, delete shaders compiled so far
			for (int i = 0; i < glShaders.size(); i++)
			{
				opengl::extensions->glDeleteObject(glShaders[i].program);
			}

			//Delete combined program
			if(m_glProgram)
				opengl::extensions->glDeleteObject(m_glProgram);
		}

		bool ShaderGLSL::CheckCompilerError(Program& program, GLenum glShader)
		{
			RendererOpenGL::CheckGLError("ShaderGLSL::CheckCompilerError() - Error compiling shader");

			GLint compiled = 0;
			opengl::extensions->glGetShaderiv(glShader, GL_COMPILE_STATUS, &compiled);
			if (compiled == GL_FALSE)
			{
				GLint maxLength = 0;
				opengl::extensions->glGetShaderiv(glShader, GL_INFO_LOG_LENGTH, &maxLength);

				if (maxLength > 0)
				{
					std::vector<GLchar> infoLog(maxLength);
					opengl::extensions->glGetShaderInfoLog(glShader, maxLength, &maxLength, infoLog.data());
					debug::error << "Error compiling shader: " << infoLog.data() << debug::end;
				}
				else
				{
					debug::error << "Error compiling shader, no log" << debug::end;
				}

				return false;
			}

			return true;
		}

		bool ShaderGLSL::ValidateProgram(GLenum glProgram)
		{
			RendererOpenGL::CheckGLError("ShaderGLSL::ValidateProgram() - Error compiling shader");

			GLint validated = 0;
			opengl::extensions->glValidateProgram(glProgram);
			opengl::extensions->glGetProgramiv(glProgram, GL_VALIDATE_STATUS, &validated);
			if (validated == GL_FALSE)
			{
				GLint maxLength = 0;
				opengl::extensions->glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &maxLength);

				if (maxLength > 0)
				{
					std::vector<GLchar> infoLog(maxLength);
					opengl::extensions->glGetShaderInfoLog(glProgram, maxLength, &maxLength, infoLog.data());
					debug::error << "Error validating shader: " << infoLog.data() << debug::end;
				}
				else
				{
					debug::error << "Error validating shader, no log" << debug::end;
				}

				return false;
			}

			return true;
		}

		bool ShaderGLSL::CheckLinkerError(GLenum glProgram)
		{
			RendererOpenGL::CheckGLError("ShaderGLSL::CheckLinkerError() - Error linking shader");

			GLint linked = 0;
			opengl::extensions->glGetProgramiv(glProgram, GL_LINK_STATUS, &linked);
			if (linked == GL_FALSE)
			{
				GLint maxLength = 0;
				opengl::extensions->glGetProgramiv(glProgram, GL_INFO_LOG_LENGTH, &maxLength);

				if (maxLength > 0)
				{
					std::vector<GLchar> infoLog(maxLength);
					opengl::extensions->glGetProgramInfoLog(glProgram, maxLength, &maxLength, infoLog.data());
					debug::error << "Error linking shader: " << infoLog.data() << debug::end;
				}
				else
				{
					debug::error << "Error linking shader, no log" << debug::end;
				}

				return false;
			}

			return true;
		}

		void ShaderGLSL::Bind()
		{
			RendererOpenGL::CheckGLContext("ShaderGLSL::Bind()");

			if (m_bindCount == 0)
			{
				ion::debug::Assert(ValidateProgram(m_glProgram), "ShaderGLSL::Bind() - Shader invalid for current GL state");
				opengl::extensions->glUseProgram(m_glProgram);
				RendererOpenGL::CheckGLError("ShaderGLSL::Bind() - Error binding shader");
			}

			m_bindCount++;
		}

		void ShaderGLSL::Unbind()
		{
			RendererOpenGL::CheckGLContext("ShaderGLSL::Unbind()");

			m_bindCount--;
			ion::debug::Assert(m_bindCount >= 0, "ShaderGLSL::Unbind() - Negative bind count");

			if (m_bindCount == 0)
			{
				opengl::extensions->glUseProgram(0);
				RendererOpenGL::CheckGLError("ShaderGLSL::Unbind() - Error unbinding shader");
			}

			for (int i = 0; i < ShaderParamDelegateGLSL::s_textureBindCount; i++)
			{
				glBindTexture(GL_TEXTURE_2D + i, 0);
			}

			ShaderParamDelegateGLSL::s_textureBindCount = 0;
		}

		Shader::ShaderParamDelegate* ShaderGLSL::CreateShaderParamDelegate(const std::string& paramName)
		{
			OpenGLContextStackLock lock;
			return new ShaderParamDelegateGLSL(opengl::extensions->glGetUniformLocation(m_glProgram, paramName.c_str()));
		}

		void ShaderGLSL::ShaderParamDelegateGLSL::Set(const int& value)
		{
			RendererOpenGL::CheckGLContext("ShaderGLSL::ShaderParamDelegateGLSL::Set()");
			opengl::extensions->glUniform1i(m_param, value);
			RendererOpenGL::CheckGLError("glUniform1i");
		}

		void ShaderGLSL::ShaderParamDelegateGLSL::Set(const float& value)
		{
			RendererOpenGL::CheckGLContext("ShaderGLSL::ShaderParamDelegateGLSL::Set()");
			opengl::extensions->glUniform1f(m_param, value);
			RendererOpenGL::CheckGLError("glUniform1f");
		}

		void ShaderGLSL::ShaderParamDelegateGLSL::Set(const Vector2& value)
		{
			RendererOpenGL::CheckGLContext("ShaderGLSL::ShaderParamDelegateGLSL::Set()");
			opengl::extensions->glUniform2f(m_param, value.x, value.y);
			RendererOpenGL::CheckGLError("glUniform2f");
		}

		void ShaderGLSL::ShaderParamDelegateGLSL::Set(const Vector3& value)
		{
			RendererOpenGL::CheckGLContext("ShaderGLSL::ShaderParamDelegateGLSL::Set()");
			opengl::extensions->glUniform3f(m_param, value.x, value.y, value.z);
			RendererOpenGL::CheckGLError("glUniform3f");
		}

		void ShaderGLSL::ShaderParamDelegateGLSL::Set(const Colour& value)
		{
			RendererOpenGL::CheckGLContext("ShaderGLSL::ShaderParamDelegateGLSL::Set()");
			opengl::extensions->glUniform4f(m_param, value.r, value.g, value.b, value.a);
			RendererOpenGL::CheckGLError("glUniform4f");
		}

		void ShaderGLSL::ShaderParamDelegateGLSL::Set(const Matrix4& value)
		{
			RendererOpenGL::CheckGLContext("ShaderGLSL::ShaderParamDelegateGLSL::Set()");
			opengl::extensions->glUniformMatrix4fv(m_param, 1, GL_FALSE, (float*)&value.GetAsFloatArray()[0]);
			RendererOpenGL::CheckGLError("glUniformMatrix4fv");
		}

		void ShaderGLSL::ShaderParamDelegateGLSL::Set(const Texture& value)
		{
			RendererOpenGL::CheckGLContext("ShaderGLSL::ShaderParamDelegateGLSL::Set()");
			glBindTexture(GL_TEXTURE_2D + s_textureBindCount, ((TextureOpenGL&)value).GetTextureId());
			opengl::extensions->glUniform1i(m_param, s_textureBindCount);
			s_textureBindCount++;
			RendererOpenGL::CheckGLError("glUniform1i");
		}

		void ShaderGLSL::ShaderParamDelegateGLSL::Set(const std::vector<float>& values)
		{
			RendererOpenGL::CheckGLContext("ShaderGLSL::ShaderParamDelegateGLSL::Set()");
			opengl::extensions->glUniform1fv(m_param, (GLsizei)values.size(), values.data());
			RendererOpenGL::CheckGLError("glUniform1fv");
		}

		void ShaderGLSL::ShaderParamDelegateGLSL::Set(const std::vector<Colour>& values)
		{
			RendererOpenGL::CheckGLContext("ShaderGLSL::ShaderParamDelegateGLSL::Set()");
			opengl::extensions->glUniform4fv(m_param, (GLsizei)values.size(), (float*)values.data());
			RendererOpenGL::CheckGLError("glUniform4fv");
		}
	}
}