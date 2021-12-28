///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		ShaderCgGL.cpp
// Date:		11th December 2013
// Authors:		Matt Phillips
// Description:	Cg shader implementation
///////////////////////////////////////////////////

#if defined ION_RENDER_SUPPORTS_CGGL

#include "core/debug/Debug.h"
#include "core/string/String.h"
#include "core/io/File.h"
#include "core/io/Archive.h"
#include "renderer/opengl/TextureOpenGL.h"
#include "renderer/opengl/RendererOpenGL.h"
#include "renderer/cggl/ShaderCgGL.h"

namespace ion
{
	namespace render
	{
		CGcontext ShaderManagerCgGL::s_cgContext = 0;
		int ShaderManagerCgGL::s_contextRefCount = 0;
		CGprofile ShaderManagerCgGL::s_cgProfileVertex;
		CGprofile ShaderManagerCgGL::s_cgProfilePixel;
		const std::string ShaderCgGL::s_languageName = "nvidiaCg";

		ShaderManager* ShaderManager::Create()
		{
			return new ShaderManagerCgGL();
		}

		ShaderManagerCgGL::ShaderManagerCgGL()
		{
			OpenGLContextStackLock lock;

			if(!s_contextRefCount)
			{
				//Set error handler
				cgSetErrorHandler(CgErrorCallback, NULL);

				s_cgContext = cgCreateContext();

				//Case sensitive semantics
				cgSetSemanticCasePolicy(CG_UNCHANGED_CASE_POLICY);

				//Set managed texture params (automatically binds/unbinds textures)
				cgGLSetManageTextureParameters(s_cgContext, true);

				//Get profiles for current GL context
				s_cgProfileVertex = cgGLGetLatestProfile(CG_GL_VERTEX);
				s_cgProfilePixel = cgGLGetLatestProfile(CG_GL_FRAGMENT);

				//Set optimal options for current GL context
				cgGLSetOptimalOptions(s_cgProfileVertex);
				cgGLSetOptimalOptions(s_cgProfilePixel);
			}

			s_contextRefCount++;
		}

		ShaderManagerCgGL::~ShaderManagerCgGL()
		{
			OpenGLContextStackLock lock;

			s_contextRefCount--;

			if(!s_contextRefCount)
			{
				cgDestroyContext(s_cgContext);
			}
		}

		void ShaderManagerCgGL::CgErrorCallback(CGcontext context, CGerror error, void* appdata)
		{
			std::string errorStr = cgGetErrorString(error);

			if(error == CG_COMPILER_ERROR)
			{
				errorStr += " : ";
				errorStr += cgGetLastListing(s_cgContext);
			}

			debug::Error(errorStr.c_str());
		}

		Shader* Shader::Create()
		{
			return new ShaderCgGL();
		}

		void Shader::RegisterSerialiseType(io::Archive& archive)
		{
			archive.RegisterPointerTypeStrict<Shader, ShaderCgGL>("ion::render::Shader");
		}

		ShaderCgGL::ShaderCgGL()
		{
			m_bindCount = 0;
			m_loaded = false;
		}

		ShaderCgGL::~ShaderCgGL()
		{
			OpenGLContextStackLock lock;

			for (int i = 0; i < m_cgPrograms.size(); i++)
			{
				cgDestroyProgram(m_cgPrograms[i].program);
			}
		}

		bool ShaderCgGL::Compile()
		{
			OpenGLContextStackLock lock;

			for (int i = 0; i < (int)ProgramType::COUNT; i++)
			{
				if (Program* program = GetProgram(ion::string::ToLower(s_languageName), (ProgramType)i))
				{
					CGprofile cgProfile = CG_PROFILE_UNKNOWN;

					switch ((ProgramType)i)
					{
					case ProgramType::Vertex:
						cgProfile = ShaderManagerCgGL::s_cgProfileVertex;
						break;
					case ProgramType::Fragment:
						cgProfile = ShaderManagerCgGL::s_cgProfilePixel;
						break;
					default:
						ion::debug::Error("ShaderCgGL::Compile() - Program type unsupported");
					}

					//Compile program
					CGprogram cgProgram = cgCreateProgram(ShaderManagerCgGL::s_cgContext, CG_SOURCE, program->m_programCode.c_str(), cgProfile, program->m_entryPoint.c_str(), NULL);

					if (program)
					{
						m_cgPrograms.push_back(NvCgProgram());
						m_cgPrograms.back().program = cgProgram;
						m_cgPrograms.back().profile = cgProfile;
					}
				}
			}

			return m_cgPrograms.size() != 0;
		}

		void ShaderCgGL::Bind()
		{
			if (m_bindCount == 0)
			{
				if (!m_loaded)
				{
					for (int i = 0; i < m_cgPrograms.size(); i++)
					{
						cgGLLoadProgram(m_cgPrograms[i].program);
					}

					m_loaded = true;
				}

				for (int i = 0; i < m_cgPrograms.size(); i++)
				{
					cgGLEnableProfile(m_cgPrograms[i].profile);
					cgGLBindProgram(m_cgPrograms[i].program);
				}
			}

			m_bindCount++;
		}

		void ShaderCgGL::Unbind()
		{
			m_bindCount--;
			ion::debug::Assert(m_bindCount >= 0, "ShaderCgGL::Unbind() - Negative bind count");

			if (m_bindCount == 0)
			{
				for (int i = 0; i < m_cgPrograms.size(); i++)
				{
					cgGLUnbindProgram(m_cgPrograms[i].profile);
					cgGLDisableProfile(m_cgPrograms[i].profile);
				}

				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
		}

		Shader::ShaderParamDelegate* ShaderCgGL::CreateShaderParamDelegate(const std::string& paramName)
		{
			ShaderParamDelegateCg* paramDelegate = NULL;

			for (int i = 0; i < m_cgPrograms.size(); i++)
			{
				CGparameter param = cgGetNamedParameter(m_cgPrograms[i].program, paramName.c_str());
				if (param)
				{
					if(!paramDelegate)
						paramDelegate = new ShaderParamDelegateCg();

					paramDelegate->m_cgParams.push_back(param);
				}
			}

			return paramDelegate;
		}

		void ShaderCgGL::ShaderParamDelegateCg::Set(const int& value)
		{
			for (int i = 0; i < m_cgParams.size(); i++)
			{
				cgGLSetParameter1f(m_cgParams[i], (float)value);
			}
		}

		void ShaderCgGL::ShaderParamDelegateCg::Set(const float& value)
		{
			for (int i = 0; i < m_cgParams.size(); i++)
			{
				cgGLSetParameter1f(m_cgParams[i], value);
			}
		}

		void ShaderCgGL::ShaderParamDelegateCg::Set(const Vector2& value)
		{
			for (int i = 0; i < m_cgParams.size(); i++)
			{
				cgGLSetParameter2fv(m_cgParams[i], (float*)&value);
			}
		}

		void ShaderCgGL::ShaderParamDelegateCg::Set(const Vector3& value)
		{
			for (int i = 0; i < m_cgParams.size(); i++)
			{
				cgGLSetParameter3fv(m_cgParams[i], (float*)&value);
			}
		}

		void ShaderCgGL::ShaderParamDelegateCg::Set(const Colour& value)
		{
			for (int i = 0; i < m_cgParams.size(); i++)
			{
				cgGLSetParameter4fv(m_cgParams[i], (float*)&value);
			}
		}

		void ShaderCgGL::ShaderParamDelegateCg::Set(const Matrix4& value)
		{
			for (int i = 0; i < m_cgParams.size(); i++)
			{
				cgSetMatrixParameterfc(m_cgParams[i], (float*)&value);
			}
		}

		void ShaderCgGL::ShaderParamDelegateCg::Set(const Texture& value)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, ((TextureOpenGL&)value).GetTextureId());

			for (int i = 0; i < m_cgParams.size(); i++)
			{
				cgGLSetTextureParameter(m_cgParams[i], ((TextureOpenGL&)value).GetTextureId());
			}
		}

		void ShaderCgGL::ShaderParamDelegateCg::Set(const std::vector<float>& values)
		{
			for (int i = 0; i < m_cgParams.size(); i++)
			{
				cgGLSetParameterArray1f(m_cgParams[i], 0, (long)values.size(), values.data());
			}
		}

		void ShaderCgGL::ShaderParamDelegateCg::Set(const std::vector<Colour>& values)
		{
			for (int i = 0; i < m_cgParams.size(); i++)
			{
				cgGLSetParameterArray4f(m_cgParams[i], 0, (long)values.size(), (float*)values.data());
			}
		}
	}
}

#endif // ION_RENDER_SUPPORTS_CGGL