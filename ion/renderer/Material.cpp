///////////////////////////////////////////////////
// (c) 2016 Matt Phillips, Big Evil Corporation
// http://www.bigevilcorporation.co.uk
// mattphillips@mail.com
// @big_evil_corp
//
// Licensed under GPLv3, see http://www.gnu.org/licenses/gpl-3.0.html
//
// File:		Material.cpp
// Date:		13th December 2011
// Authors:		Matt Phillips
// Description:	Material class and file loader
///////////////////////////////////////////////////

#include "renderer/Material.h"
#include "core/debug/Debug.h"

#include <algorithm>

namespace ion
{
	namespace render
	{
		Material::Material()
		{
			//Set default lighting and shadows
			SetLightingEnabled(false);
			SetLightingMode(LightingMode::Phong);
			SetBlendMode(BlendMode::Additive);
			SetReceiveShadows(false);
		}

		Material::~Material()
		{
		}

#if defined ION_RENDERER_SHADER
		void Material::SetShader(io::ResourceHandle<Shader> shader)
		{
			m_shader = shader;

			if(shader)
			{
				m_shaderParams.matrices.world = shader->CreateParamHndl<Matrix4>("gWorldMatrix");
				m_shaderParams.matrices.view = shader->CreateParamHndl<Matrix4>("gViewMatrix");
				m_shaderParams.matrices.worldView = shader->CreateParamHndl<Matrix4>("gWorldViewMatrix");
				m_shaderParams.matrices.worldViewProjection = shader->CreateParamHndl<Matrix4>("gWorldViewProjectionMatrix");
				m_shaderParams.matrices.normal = shader->CreateParamHndl<Matrix4>("gNormalMatrix");
				m_shaderParams.colours.ambient = shader->CreateParamHndl<Colour>("gAmbientColour");
				m_shaderParams.colours.diffuse = shader->CreateParamHndl<Colour>("gDiffuseColour");
				m_shaderParams.colours.specular = shader->CreateParamHndl<Colour>("gSpecularColour");
				m_shaderParams.colours.emissive = shader->CreateParamHndl<Colour>("gEmissiveColour");
				m_shaderParams.textures.diffuseMap = shader->CreateParamHndl<Texture>("gDiffuseTexture");
				m_shaderParams.textures.normalMap = shader->CreateParamHndl<Texture>("gNormalTexture");
				m_shaderParams.textures.opacityMap = shader->CreateParamHndl<Texture>("gOpacityTexture");
				m_shaderParams.textures.specularMap = shader->CreateParamHndl<Texture>("gSpecularTexture");
			}
		}

		io::ResourceHandle<Shader> Material::GetShader() const
		{
			return m_shader;
		}

		Material::ShaderParams& Material::GetShaderParams()
		{
			return m_shaderParams;
		}
#endif

		void Material::RegisterSerialiseType(io::Archive& archive)
		{
			archive.RegisterPointerType<Material>("ion::render::Material");
		}

		void Material::Serialise(io::Archive& archive)
		{
			//Colours
			archive.Serialise(m_ambientColour);
			archive.Serialise(m_diffuseColour);
			archive.Serialise(m_specularColour);
			archive.Serialise(m_emissiveColour);

			//Textures
			archive.Serialise(m_diffuseMaps);
			archive.Serialise(m_normalMap);
			archive.Serialise(m_specularMap);
			archive.Serialise(m_opacityMap);

			//TODO
			//Shaders
			//archive.Serialise(m_shader);

			//Params
			archive.Serialise(m_lightingEnabled);
			archive.Serialise(m_receiveShadows);
			archive.Serialise((u32&)m_lightingMode);
			archive.Serialise((u32&)m_blendMode);

			if(archive.GetDirection() == io::Archive::Direction::In)
			{
				//SetShader(m_shader);
			}
		}

		void Material::SetBlendMode(BlendMode blendMode)
		{
			m_blendMode = blendMode;
		}

		Material::BlendMode Material::GetBlendMode() const
		{
			return m_blendMode;
		}

		void Material::SetAmbientColour(const Colour& ambient)
		{
			m_ambientColour = ambient;
		}

		void Material::SetDiffuseColour(const Colour& diffuse)
		{
			m_diffuseColour = diffuse;
		}

		void Material::SetSpecularColour(const Colour& specular)
		{
			m_specularColour = specular;
		}

		void Material::SetEmissiveColour(const Colour& emissive)
		{
			m_emissiveColour = emissive;
		}

		const Colour& Material::GetAmbientColour() const
		{
			return m_ambientColour;
		}

		const Colour& Material::GetDiffuseColour() const
		{
			return m_diffuseColour;
		}

		const Colour& Material::GetSpecularColour() const
		{
			return m_specularColour;
		}

		const Colour& Material::GetEmissiveColour() const
		{
			return m_emissiveColour;
		}

		void Material::AddDiffuseMap(io::ResourceHandle<Texture>& diffuse)
		{
			m_diffuseMaps.push_back(diffuse);
		}

		void Material::SetDiffuseMap(io::ResourceHandle<Texture>& diffuse, int diffuseMapIdx)
		{
			m_diffuseMaps.insert(m_diffuseMaps.begin() + diffuseMapIdx, diffuse);
		}

		void Material::SetNormalMap(io::ResourceHandle<Texture>& normal)
		{
			m_normalMap = normal;
		}

		void Material::SetSpecularMap(io::ResourceHandle<Texture>& specular)
		{
			m_specularMap = specular;
		}

		void Material::SetOpacityMap(io::ResourceHandle<Texture>& opacity)
		{
			m_opacityMap = opacity;
		}

		io::ResourceHandle<Texture> Material::GetDiffuseMap(int diffuseMapIdx) const
		{
			if(diffuseMapIdx < (int)m_diffuseMaps.size())
			{
				return m_diffuseMaps[diffuseMapIdx];
			}

			return io::ResourceHandle<Texture>();
		}

		io::ResourceHandle<Texture> Material::GetNormalMap() const
		{
			return m_normalMap;
		}

		io::ResourceHandle<Texture> Material::GetSpecularMap() const
		{
			return m_specularMap;
		}

		io::ResourceHandle<Texture> Material::GetOpacityMap() const
		{
			return m_opacityMap;
		}

		int Material::GetNumDiffuseMaps() const
		{
			return (int)m_diffuseMaps.size();
		}

		void Material::SetLightingEnabled(bool lighting)
		{
			m_lightingEnabled = lighting;
		}

		bool Material::GetLightingEnabled() const
		{
			return m_lightingEnabled;
		}

		void Material::SetLightingMode(LightingMode mode)
		{
			m_lightingMode = mode;
		}

		Material::LightingMode Material::GetLightingMode() const
		{
			return m_lightingMode;
		}

		void Material::SetReceiveShadows(bool shadows)
		{
			m_receiveShadows = shadows;
		}

		bool Material::GetReceiveShadows() const
		{
			return m_receiveShadows;
		}

		void Material::SetDepthTest(bool enabled)
		{

		}

		void Material::SetDepthWrite(bool enabled)
		{

		}

		void Material::SetCullMode(CullMode cullMode)
		{

		}
	}
}
