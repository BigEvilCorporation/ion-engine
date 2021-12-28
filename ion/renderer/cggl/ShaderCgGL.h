///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		ShaderCgGL.h
// Date:		11th December 2013
// Authors:		Matt Phillips
// Description:	Cg shader implementation
///////////////////////////////////////////////////

#pragma once

#if defined ION_RENDER_SUPPORTS_CGGL

#include "renderer/Shader.h"

#include "OpenGL/OpenGLInclude.h"

#include <Windows.h>
#include <WinDef.h>
#include <Wingdi.h>

namespace ion
{
	namespace render
	{
		class ShaderManagerCgGL : public ShaderManager
		{
		public:
			ShaderManagerCgGL();
			virtual ~ShaderManagerCgGL();

			static CGcontext s_cgContext;
			static int s_contextRefCount;

			static CGprofile s_cgProfileVertex;
			static CGprofile s_cgProfilePixel;

		private:
			static void CgErrorCallback(CGcontext context, CGerror error, void* appdata);
		};

		class ShaderCgGL : public Shader
		{
		public:
			ShaderCgGL();
			virtual ~ShaderCgGL();

			//Compile shader
			virtual bool Compile();

			//Bind/unbind
			virtual void Bind();
			virtual void Unbind();

			static const std::string s_languageName;

		protected:

			class ShaderParamDelegateCg : public ShaderParamDelegate
			{
			public:
				virtual void Set(const int& value);
				virtual void Set(const float& value);
				virtual void Set(const Vector2& value);
				virtual void Set(const Vector3& value);
				virtual void Set(const Colour& value);
				virtual void Set(const Matrix4& value);
				virtual void Set(const Texture& value);

				virtual void Set(const std::vector<float>& values);
				virtual void Set(const std::vector<Colour>& values);

				std::vector<CGparameter> m_cgParams;
			};

			virtual ShaderParamDelegate* CreateShaderParamDelegate(const std::string& paramName);

			struct NvCgProgram
			{
				CGprogram program;
				CGprofile profile;
			};

			std::vector<NvCgProgram> m_cgPrograms;
			bool m_loaded;
			int m_bindCount;
		};
	}
}

#endif // ION_RENDER_SUPPORTS_CGGL