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
#include "io/Archive.h"
#include "io/ResourceHandle.h"

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

			enum BlendMode
			{
				eAdditive,
				eModulative,
				eReplace,
				eTransparentAlpha,
				eTransparentColour
			};

			enum LightingMode
			{
				eFlat,
				eGouraud,
				ePhong
			};

			enum CullMode
			{
				eNone,
				eClockwise,
				eCounterClockwise
			};

			enum ColourType
			{
				eAmbient,
				eDiffuse,
				eSpecular,
				eEmissive
			};

			Material();
			~Material();

			//Bind/unbind
			void Bind(const Matrix4& worldMtx, const Matrix4& viewMtx, const Matrix4& projectionMtx);
			void Unbind();

			//Shaders
#if defined ION_RENDERER_SHADER
			void SetVertexShader(Shader* vertexShader);
			void SetPixelShader(Shader* pixelShader);
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
			void AddDiffuseMap(Texture* diffuse);
			void SetDiffuseMap(Texture* diffuse, int diffuseMapIdx);
			void SetNormalMap(Texture* normal);
			void SetSpecularMap(Texture* specular);
			void SetOpacityMap(Texture* opacity);

			Texture* GetDiffuseMap(int diffuseMapIdx) const;
			Texture* GetNormalMap() const;
			Texture* GetSpecularMap() const;
			Texture* GetOpacityMap() const;

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

#if defined ION_RENDERER_SHADER
			void ApplyShaderParams(const Matrix4& worldMtx, const Matrix4& viewMtx, const Matrix4& projectionMtx);
#endif

			Colour m_ambientColour;
			Colour m_diffuseColour;
			Colour m_specularColour;
			Colour m_emissiveColour;

			std::vector<Texture*> m_diffuseMaps;
			Texture* m_normalMap;
			Texture* m_specularMap;
			Texture* m_opacityMap;

			bool m_lightingEnabled;
			bool m_receiveShadows;
			LightingMode m_lightingMode;

			BlendMode m_blendMode;

#if defined ION_RENDERER_SHADER
			Shader* m_vertexShader;
			Shader* m_pixelShader;

			struct ShaderParams
			{
				struct MatrixParams
				{
					Shader::ParamHndl<Matrix4> m_world;
					Shader::ParamHndl<Matrix4> m_worldViewProjection;
				} m_matrices;

				struct ColourParams
				{
					Shader::ParamHndl<Colour> m_ambient;
					Shader::ParamHndl<Colour> m_diffuse;
					Shader::ParamHndl<Colour> m_specular;
					Shader::ParamHndl<Colour> m_emissive;
				} m_colours;

				struct TextureParams
				{
					Shader::ParamHndl<Texture> m_diffuseMap;
					Shader::ParamHndl<Texture> m_normalMap;
					Shader::ParamHndl<Texture> m_specularMap;
					Shader::ParamHndl<Texture> m_opacityMap;
				} m_textures;
			};

			ShaderParams m_vertexShaderParams;
			ShaderParams m_pixelShaderParams;
#endif
		};
	}
}