///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Material.h
// Date:		13th December 2011
// Authors:		Matt Phillips
// Description:	Material class and file loader
///////////////////////////////////////////////////

#pragma once

#include "core/Types.h"
#include "maths/Matrix.h"
#include "renderer/Colour.h"
#include "renderer/Texture.h"
#include "core/io/Archive.h"
#include "resource/ResourceHandle.h"

#if defined ION_RENDERER_SHADER
#include "renderer/Shader.h"
#endif

#include <string>
#include <vector>

namespace ion
{
	namespace render
	{
		class Material
		{
		public:

			enum class BlendMode
			{
				Additive,
				Modulative,
				Replace,
				TransparentAlpha,
				TransparentColour
			};

			enum class LightingMode
			{
				Flat,
				Gouraud,
				Phong
			};

			enum class CullMode
			{
				None,
				Clockwise,
				CounterClockwise
			};

			enum class ColourType
			{
				Ambient,
				Diffuse,
				Specular,
				Emissive
			};

#if defined ION_RENDERER_SHADER
			struct ShaderParams
			{
				struct MatrixParams
				{
					Shader::ParamHndl<Matrix4> world;
					Shader::ParamHndl<Matrix4> view;
					Shader::ParamHndl<Matrix4> worldView;
					Shader::ParamHndl<Matrix4> worldViewProjection;
					Shader::ParamHndl<Matrix4> normal;
				} matrices;

				struct ColourParams
				{
					Shader::ParamHndl<Colour> ambient;
					Shader::ParamHndl<Colour> diffuse;
					Shader::ParamHndl<Colour> specular;
					Shader::ParamHndl<Colour> emissive;
				} colours;

				struct TextureParams
				{
					Shader::ParamHndl<Texture> diffuseMap;
					Shader::ParamHndl<Texture> normalMap;
					Shader::ParamHndl<Texture> specularMap;
					Shader::ParamHndl<Texture> opacityMap;
				} textures;
			};
#endif

			Material();
			~Material();

			//Shaders
#if defined ION_RENDERER_SHADER
			void SetShader(io::ResourceHandle<Shader> shader);
			io::ResourceHandle<Shader> GetShader() const;
			ShaderParams& GetShaderParams();
#endif

			//Colour
			void SetAmbientColour(const Colour& ambient);
			void SetDiffuseColour(const Colour& diffuse);
			void SetSpecularColour(const Colour& specular);
			void SetEmissiveColour(const Colour& emissive);

			const Colour& GetAmbientColour() const;
			const Colour& GetDiffuseColour() const;
			const Colour& GetSpecularColour() const;
			const Colour& GetEmissiveColour() const;

			//Texture maps
			void AddDiffuseMap(io::ResourceHandle<Texture>& diffuse);
			void SetDiffuseMap(io::ResourceHandle<Texture>& diffuse, int diffuseMapIdx);
			void SetNormalMap(io::ResourceHandle<Texture>& normal);
			void SetSpecularMap(io::ResourceHandle<Texture>& specular);
			void SetOpacityMap(io::ResourceHandle<Texture>& opacity);

			io::ResourceHandle<Texture> GetDiffuseMap(int diffuseMapIdx) const;
			io::ResourceHandle<Texture> GetNormalMap() const;
			io::ResourceHandle<Texture> GetSpecularMap() const;
			io::ResourceHandle<Texture> GetOpacityMap() const;

			int GetNumDiffuseMaps() const;

			//Lighting and shadows
			void SetLightingEnabled(bool lighting);
			void SetLightingMode(LightingMode mode);
			void SetReceiveShadows(bool shadows);

			bool GetLightingEnabled() const;
			LightingMode GetLightingMode() const;
			bool GetReceiveShadows() const;

			//Blend mode
			void SetBlendMode(BlendMode blendMode);
			BlendMode GetBlendMode() const;

			//Depth and culling
			void SetDepthTest(bool enabled);
			void SetDepthWrite(bool enabled);
			void SetCullMode(CullMode cullMode);

			//Serialisation
			static void RegisterSerialiseType(io::Archive& archive);
			void Serialise(io::Archive& archive);

		protected:
			Colour m_ambientColour;
			Colour m_diffuseColour;
			Colour m_specularColour;
			Colour m_emissiveColour;

			std::vector<io::ResourceHandle<Texture>> m_diffuseMaps;
			io::ResourceHandle<Texture> m_normalMap;
			io::ResourceHandle<Texture> m_specularMap;
			io::ResourceHandle<Texture> m_opacityMap;

			bool m_lightingEnabled;
			bool m_receiveShadows;
			LightingMode m_lightingMode;

			BlendMode m_blendMode;

#if defined ION_RENDERER_SHADER
			io::ResourceHandle<Shader> m_shader;
			ShaderParams m_shaderParams;
#endif
		};
	}
}